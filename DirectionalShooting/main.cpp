#include "Gathering_Utility.h"
#include "Gathering_Node.h"
#include "RoughINIReader.h"
//#include "utilities/IConsole.h"
//NoGore is unsupported in xNVSE


IDebugLog		gLog("DirectionalShooting.log");
PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
NVSEMessagingInterface* g_messagingInterface{};
NVSEInterface* g_nvseInterface{};
NVSEEventManagerInterface* g_eventInterface{};
_InventoryRefGetForID InventoryRefGetForID;
_InventoryRefCreate InventoryRefCreate;

static CallDetour AttackingCheckInWobble{};
static CallDetour InitProjRotForAutoAim{};
static CallDetour DealProjImpact{};
static CallDetour PC_BeforeDoAttackAnim{};	// 949CF1

struct Rectifier {
	bool doRectifyRot{ false };
	bool doRectifyPos{ false };
	NiVector3 rectified_Pos{ 0,0,0 };
	float rectified_RotX{ 0 };
	float rectified_RotZ{ 0 };

	float PC_RotX{ 0 };
	float PC_RotZ{ 0 };

};

enum class CONFIG_FLAG : UINT32 {
	CFG_HIPFIRE = 0x0001,
	CFG_IS = 0x0002,
	CFG_ThrowableMelee = 0x0004,
};

static inline OptFlag<CONFIG_FLAG> config_flag{};
static inline Rectifier g_rectifier{};

bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	_MESSAGE("query");

	// fill out the info structure
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "DirectionalShooting";
	info->version = 114;

	// version checks
	if (nvse->nvseVersion < PACKED_NVSE_VERSION)
	{
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, PACKED_NVSE_VERSION);
		return false;
	}

	if (!nvse->isEditor)
	{
		if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
		{
			_ERROR("incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
			return false;
		}

		if (nvse->isNogore)
		{
			_ERROR("NoGore is not supported");
			return false;
		}
	}
	else
	{
		if (nvse->editorVersion < CS_VERSION_1_4_0_518)
		{
			_ERROR("incorrect editor version (got %08X need at least %08X)", nvse->editorVersion, CS_VERSION_1_4_0_518);
			return false;
		}
	}

	// version checks pass
	// any version compatibility checks should be done here
	return true;
}

namespace Ns_DirectionalShoot {

	static auto __forceinline WpRotMappingToWldRot() {
		struct wrapper {
			float mappingZ;
			float mappingX;
		};

		auto* _pc = PlayerCharacter::GetSingleton();
		if (NiNode* _1stNode = _pc->node1stPerson)
		{
			
			if (NiAVObject* _block = _1stNode->GetBlock("Weapon")) 
			{
				NiVector4 _eAngInv = getWorldRotationAlt(_block, 3);
				_eAngInv.z += 180;
				float mappingZ = std::fmod((270 - _eAngInv.z), 360.0);
				if (mappingZ < 0)
					mappingZ = 360 + mappingZ;
				/*gLog.FormattedMessage("Weapon XROT: %.5f, YROT: %.5f, ZROT: %.5f",  
					_eAngInv.y , _eAngInv.x, _eAngInv.z);*/
				
				
				return wrapper{ mappingZ ,_eAngInv.y };
			}
			
		}
		return wrapper{ FLT_MIN,FLT_MIN };
	}


	static bool __fastcall _BeforePCAttackAnimation(Actor* _actor, void* _edx, int animGrup) {
		//if (_actor && _actor->refID == 0x14 && IsEquipRanged(_actor) && !IsUnarmOrWeaponIsNull(_actor) )
		if (_actor && _actor->refID == 0x14 && !IsUnarmOrWeaponIsNull(_actor) && IsEquipNotMelee(_actor))
		{
			g_rectifier.doRectifyPos = false;
			g_rectifier.doRectifyRot = false;

			g_rectifier.PC_RotX = _actor->rotX;
			g_rectifier.PC_RotZ = _actor->rotZ;

			auto* _pc = PlayerCharacter::GetSingleton();
			NiNode* _pjnode = GetPC1stPJNode();
			if (_pjnode) {
				g_rectifier.doRectifyPos = true;
				g_rectifier.rectified_Pos = _pjnode->WorldTranslate();
			}

			bool isAiming = isActorAiming(_pc);

			// Aiming, has sightingNode and without B42 Optics / Scope
			if (isAiming) {
				if (config_flag.IsFlagOn(CONFIG_FLAG::CFG_IS) && PCHasSightingNode()) {
					if (!IsUsingB42OpticsLens() && !GetPCUsingScope()) {
						g_rectifier.doRectifyRot = true;
					}
				}
			}
			// hip fire
			else {
				if (config_flag.IsFlagOn(CONFIG_FLAG::CFG_HIPFIRE)) {
					g_rectifier.doRectifyRot = true;
				}
			}

			if (g_rectifier.doRectifyRot)
			{
				if (auto WpRotMapping = WpRotMappingToWldRot(); WpRotMapping.mappingZ != FLT_MIN)
				{
					WpRotMapping.mappingZ /= DeToAg;
					WpRotMapping.mappingX /= DeToAg;

					g_rectifier.rectified_RotZ = WpRotMapping.mappingZ;
					g_rectifier.rectified_RotX = WpRotMapping.mappingX;

					//gLog.FormattedMessage("Rectify RX = %.5f, RZ = %.5f", g_rectifier.rectified_RotX, g_rectifier.rectified_RotZ);
					return ThisStdCall<bool>(PC_BeforeDoAttackAnim.GetOverwrittenAddr(), _actor, animGrup);
				}
			}
		}

		return ThisStdCall<bool>(PC_BeforeDoAttackAnim.GetOverwrittenAddr(), _actor, animGrup);
	}

	static void __fastcall _DirectionalShoot(PlayerCharacter* _pc, void* _edx, Projectile* _proj, NiNode* _projNodeInWeapon,
		float* _pj_RotZ, float* _pj_RotX, NiVector3* _pj_Pos, float _ngtZeroZ, float _ngtZeroX) {

		if (!_pc || _pc->refID != 0x14 || !_proj || _pc->IsThirdPerson()) {
			
			
			
			g_rectifier.doRectifyPos = false;
			g_rectifier.doRectifyRot = false;
			ThisStdCall<void>(InitProjRotForAutoAim.GetOverwrittenAddr(), _pc, _proj, _projNodeInWeapon, _pj_RotZ, _pj_RotX, _pj_Pos, _ngtZeroZ, _ngtZeroX);
			return;
		}

		if (IsEquipThrowableMelee(_pc)){
			ThisStdCall<void>(InitProjRotForAutoAim.GetOverwrittenAddr(), _pc, _proj, _projNodeInWeapon, _pj_RotZ, _pj_RotX, _pj_Pos, _ngtZeroZ, _ngtZeroX);
			if (auto WpRotMapping = WpRotMappingToWldRot(); WpRotMapping.mappingZ != FLT_MIN)
			{
				_proj->rotX = WpRotMapping.mappingX / DeToAg;
				_proj->rotZ = WpRotMapping.mappingZ / DeToAg;
			}


			return;
		}

		float pc_rotX = _pc->rotX;
		float pc_rotZ = _pc->rotZ;
		// spread
		float offset_rX = *_pj_RotX - _pc->rotX;
		float offset_rZ = *_pj_RotZ - _pc->rotZ;
		ThisStdCall<void>(InitProjRotForAutoAim.GetOverwrittenAddr(), _pc, _proj, _projNodeInWeapon, _pj_RotZ, _pj_RotX, _pj_Pos, _ngtZeroZ, _ngtZeroX);
		if (g_rectifier.doRectifyPos) {
			g_rectifier.doRectifyPos = false;
			_proj->posX = g_rectifier.rectified_Pos.x;
			_proj->posY = g_rectifier.rectified_Pos.y;
			_proj->posZ = g_rectifier.rectified_Pos.z;
		}/*
		gLog.FormattedMessage("Before PCRX - %.5f, PCRZ - %.5f", pc_rotX, pc_rotZ);
		gLog.FormattedMessage("Before RotX - %.5f, RotY - %.5f",_proj->rotX, _proj->rotZ);*/
		if (g_rectifier.doRectifyRot) {
			g_rectifier.doRectifyRot = false;


			_proj->rotX = g_rectifier.rectified_RotX + offset_rX;
			_proj->rotZ = g_rectifier.rectified_RotZ + offset_rZ;

			if (IsEquipThrowableMelee(_pc)) {
				_proj->rotX += (pc_rotX - g_rectifier.PC_RotX);
				_proj->rotZ += (pc_rotZ - g_rectifier.PC_RotZ);
			}
			
			//gLog.FormattedMessage("After RotX: %.5f, RotZ: %.5f", _proj->rotX, _proj->rotZ);
		}
		/*gLog.FormattedMessage("After RotX - %.5f, RotY - %.5f", _proj->rotX, _proj->rotZ);*/
	}

	// test
	static void __fastcall _DealImpact(Projectile* _this,void* edx, TESObjectREFR* _ref,
										NiVector3* _pos,NiVector3* _rot, void* cdBody,UINT8 _mType) {
		if (_this)
		{
			gLog.FormattedMessage("IMPACT ROT X: %.5f, Z:%.5f", _this->rotX, _this->rotZ);
		}
		ThisStdCall<void>(DealProjImpact.GetOverwrittenAddr(),_this, _ref,_pos,_rot,cdBody,_mType);
	}

	// enable wobble when fire. test
	static bool __fastcall IsDoingAttckAnimation(Actor* _this) {
		ThisStdCall<bool>(AttackingCheckInWobble.GetOverwrittenAddr(), _this);
		return false;
	}

	static inline void InstallHook()
	{
		//AttackingCheckInWobble.WriteRelCall(0x96306B,UINT32(IsDoingAttckAnimation));
		InitProjRotForAutoAim.WriteRelCall(0x9BD9E2, UINT32(_DirectionalShoot));
		PC_BeforeDoAttackAnim.WriteRelCall(0x949CF1, UINT32(_BeforePCAttackAnimation));
		//DealProjImpact.WriteRelCall(0x9C2058, UINT32(_DealImpact));
	}

	namespace fs = std::filesystem;
	static inline bool ReadGenericConfig() {
		gLog.Message("ReadGenericConfig");
		fs::path config_root_path = fs::current_path();
		config_root_path += R"(\Data\NVSE\Plugins\DirectionShootingConfig\)";
		if (!fs::exists(config_root_path)) {
			gLog.Message("ReadGenericConfig path not exist");
			return false;
		}

		roughinireader::INIReader _ini{ config_root_path };
		auto ret = _ini.SetCurrentINIFileName("DirectionShootConfig.ini");
		if (!ret.has_value()) {
			gLog.FormattedMessage("Failed to set generic config filename : %s", ret.error().message());
			return false;
		}
		ret = _ini.ConstructSectionMap();
		if (!ret.has_value()) {
			gLog.FormattedMessage("Failed to construct section map : %s", ret.error().message());
			return false;
		}


		std::string raw_type_val = "";
		UINT32 temp_flag = 0;
		raw_type_val = _ini.GetRawTypeVal("General", "HipFireRectify");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		gLog.FormattedMessage(raw_type_val.c_str());
		if (temp_flag != 0)
			config_flag.SetFlagOn(CONFIG_FLAG::CFG_HIPFIRE);

		raw_type_val = _ini.GetRawTypeVal("General", "AimingRectify");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		gLog.FormattedMessage(raw_type_val.c_str());
		if (temp_flag != 0)
			config_flag.SetFlagOn(CONFIG_FLAG::CFG_IS);

		raw_type_val = _ini.GetRawTypeVal("General", "ThrowableMeleeRectify");
		temp_flag = raw_type_val.empty() ? 0 : static_cast<UINT32>(std::stoi(raw_type_val));
		gLog.FormattedMessage(raw_type_val.c_str());
		if (temp_flag != 0)
			config_flag.SetFlagOn(CONFIG_FLAG::CFG_ThrowableMelee);

		gLog.FormattedMessage("config %d", config_flag.flag);
	}
}


// This is a message handler for nvse events
// With this, plugins can listen to messages such as whenever the game loads
void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_DeferredInit:
		initSingleton();
		Ns_DirectionalShoot::InstallHook();
		Ns_DirectionalShoot::ReadGenericConfig();
		break;
	case NVSEMessagingInterface::kMessage_MainGameLoop:
	{
	}
	break;
	}
}

bool NVSEPlugin_Load(NVSEInterface* nvse)
{
	g_pluginHandle = nvse->GetPluginHandle();
	// save the NVSE interface in case we need it later
	g_nvseInterface = nvse;
	NVSEDataInterface* nvseData = (NVSEDataInterface*)nvse->QueryInterface(kInterface_Data);
	InventoryRefGetForID = (_InventoryRefGetForID)nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceGetForRefID);
	InventoryRefCreate = (_InventoryRefCreate)nvseData->GetFunc(NVSEDataInterface::kNVSEData_InventoryReferenceCreateEntry);

	// register to receive messages from NVSE

	if (!nvse->isEditor)
	{
		g_messagingInterface = static_cast<NVSEMessagingInterface*>(nvse->QueryInterface(kInterface_Messaging));
		g_messagingInterface->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);
	}
	return true;
}