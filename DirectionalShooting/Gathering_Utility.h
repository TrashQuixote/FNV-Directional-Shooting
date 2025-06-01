#pragma once
#include "Gathering_Code.h"
#include "Gathering_Node.h"

template<typename TYPE>
struct OptFlag{
    UINT32 flag;
    bool IsFlagOn(TYPE _flag) const {
        return (flag & static_cast<UINT32>(_flag)) != 0;
    }

    void SetFlagOn(TYPE _flag) {
        flag |= static_cast<UINT32>(_flag);
    }
};

template<typename T>
constexpr int sgn(const T& a, const T& b) noexcept {
    return (a > b) - (a < b);
}

template<typename T>
constexpr int sgn(const T& a) noexcept {
    return sgn(a, T(0));
}

/*
===== AngleCompensation =====
deal with the situation when the rot > 3.14
*/
#define AngleCompensationZ(z) z -= (Double_PI * (-sgn(z))) 

#define AngleCompensationX(x) x = (PI - std::fabs(x)) * sgn(x)

#define PC_Ref PlayerCharacter::GetSingleton()
#define Ut_Square(x) ( (x) * (x) )
constexpr UINT32 cdecl_getSubNodeByNameAddr = 0x4AAE30;
constexpr UINT32 thiscall_isActorAiming = 0x8BBC10;
constexpr UINT32 cdecl_isISAnimation = 0x5F2750;
float constexpr DeToAg = 57.29578018;
float constexpr PI = 3.1415926;
float constexpr Double_PI = 6.2831852;
float constexpr Half_PI = 1.5707963;

static __forceinline NiNode* GetActorPJNode(Actor* _actor) {
    if (_actor && _actor->baseProcess && !_actor->baseProcess->processLevel) return _actor->baseProcess->GetProjectileNode();
    return nullptr;
}

enum HitLocation {
    HL_None,
    Torso,
    Head,
    Arm,
    Leg,
    Weapon
};

static HitLocation GetHitLimbLocation(SInt32 hit_location) {
    switch (hit_location)
    {
    case 0:
        return HitLocation::Torso;
    case 1:
    case 2:
    case 13:
        return HitLocation::Head;
    case 3:
    case 4:
    case 5:
    case 6:
        return HitLocation::Arm;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        return HitLocation::Leg;
    default:
        return HitLocation::HL_None;
    }
}

static __forceinline NiVector4 getWorldRotationAlt(NiAVObject* niBlock,UInt32 getMode) {
    NiVector4 rot;
    __asm
    {
        mov		ecx, niBlock
        lea		eax, [ecx + 0x68]
        add		ecx, 0x34
        test	getMode, 1
        cmovnz	ecx, eax
        mov		eax, NiMatrix33::ToEulerPRY
        mov		edx, NiMatrix33::ToEulerPRYInv
        test	getMode, 2
        cmovnz	eax, edx
        call	eax
        mulps	xmm0, PS_V3_180dPI
        movups	rot, xmm0
    }
    return NiVector4{rot.x,rot.y,rot.z,rot.w};
}



static __forceinline bool GetPCUsingScope() {
    return HUDMainMenu::Get()->isUsingScope ? true : false;
}

static __forceinline auto printWorldRotation(const NiMatrix33& _this) {
    float roll = _this.ExtractRoll();
    float yaw = _this.ExtractYaw();
    float pitch = _this.ExtractPitch();
    gLog.FormattedMessage("pitch %.5f,roll %.5f,yaw %.5f", pitch * DeToAg, roll * DeToAg, yaw * DeToAg);
}

static __forceinline auto printWorldRotation(NiNode* _this) {
    printWorldRotation(_this->m_transformWorld.rotate);
}

static __forceinline bool isActorAiming(Actor* _act) {
    return ThisStdCall<bool>(thiscall_isActorAiming,_act);
}

static __forceinline bool SetStartingPos(TESObjectREFR* _ref,const NiVector3& _startPos) {
    ExtraStartingPosition* start_pos = static_cast<ExtraStartingPosition*>(_ref->extraDataList.GetByType(kExtraData_StartingPosition));
    if (!start_pos) return false;
    start_pos->posVector.x = _startPos.x;
    start_pos->posVector.y = _startPos.y;
    start_pos->posVector.z = _startPos.z;
    return true;
}

static __forceinline bool PCHasBlock(const char* _blockName) {
    if (auto* _pc = PlayerCharacter::GetSingleton())
        if (auto* _1stNode = _pc->node1stPerson)
            if (auto* _sNode = _1stNode->GetBlock(_blockName))
                return true;
    return false;
}

static __forceinline bool IsUsingB42OpticsLens() {
    return PCHasBlock("OldIS");
}

static __forceinline float NormalizedAngleZ(float _rawZ) {
    if (_rawZ < -180)
        _rawZ += 360;
    else if (_rawZ > 180)
        _rawZ -= 360;
    return _rawZ;
}

static __forceinline bool PCHasSightingNode() {
    return PCHasBlock("##SightingNode");
}

static __forceinline float GetHeadingAngleX_PosBased(const NiVector3& _outPos, float _outRotX,const NiVector3& _toPos) {
    float opposite = _outPos.z - _toPos.z;
    float hypotenuse = Point3Distance(_outPos, _toPos);
    float fraction = opposite / hypotenuse;
    return (asin(fraction) - _outRotX) * Flt180dPI;
}

static __forceinline float GetHeadingAngleZ_PosBased (const NiVector3& _outPos, float _outRotZ, const NiVector3& _toPos)
{
    float result = (atan2(_toPos.x - _outPos.x, _toPos.y - _outPos.y) - _outRotZ) * 57.29578f;
    if (result < -180)
        result += 360;
    else if (result > 180)
        result -= 360;
    return result;
}

static __forceinline float GetHeadingAngleZBetweenNode(const NiNode* _outNd,float _outRZ,const NiNode* _toNd) {
    return GetHeadingAngleZ_PosBased(_outNd->m_transformWorld.translate,_outRZ,_toNd->m_transformWorld.translate);
}

static __forceinline float GetHeadingAngleXBetweenNode(const NiNode* _outNd, float _outRX, const NiNode* _toNd) {
    return GetHeadingAngleZ_PosBased(_outNd->m_transformWorld.translate, _outRX, _toNd->m_transformWorld.translate);
}

// only use _proj to calc
static __forceinline float GetImpactPosHeadingAngleToProj_X(Projectile* _proj,float _rotX) {
    const auto& _proj_direction = _proj->UnitVector;
    const auto _proj_cur_pos = _proj->GetPos();
    NiVector3 offset_pos{ _proj_cur_pos->x - _proj_direction.x,_proj_cur_pos->y - _proj_direction.y,_proj_cur_pos->z - _proj_direction.z };

    return GetHeadingAngleX_PosBased(*_proj_cur_pos, _rotX, offset_pos);
}

static __forceinline float GetImpactPosHeadingAngleToProj_Z(Projectile* _proj,float _rotZ) {
    const auto _proj_direction = _proj->UnitVector;
    const auto _proj_cur_pos = _proj->GetPos();
    NiVector3 offset_pos{ _proj_cur_pos->x - _proj_direction.x,_proj_cur_pos->y - _proj_direction.y,_proj_cur_pos->z - _proj_direction.z };

    return GetHeadingAngleX_PosBased(*_proj_cur_pos, _rotZ, offset_pos);
}

namespace ActorDTDR_Details {
    struct CreatureDTDR {
        DTDR native_dtdr;

        float __forceinline GetNativeDT() const { return native_dtdr.DT; }
        float __forceinline GetNativeDR() const { return native_dtdr.DR; }
    };

    struct CharacterDTDR {
        DTDR armor_dtdr;
        DTDR helmet_dtdr;

        float __forceinline GetHelmetDT() const { return helmet_dtdr.DT; }
        float __forceinline GetHelmetDR() const { return helmet_dtdr.DR; }
        float __forceinline GetArmorDT() const { return armor_dtdr.DT; }
        float __forceinline GetArmorDR() const { return armor_dtdr.DR; }
        float __forceinline GetTotalDT() const { return armor_dtdr.DT + helmet_dtdr.DT; }
        float __forceinline GetTotalDR() const { return armor_dtdr.DR + helmet_dtdr.DR; }
    };
};
using CreatureDTDR = ActorDTDR_Details::CreatureDTDR;
using CharacterDTDR = ActorDTDR_Details::CharacterDTDR;
using ActorDTDR = std::variant<std::monostate, CharacterDTDR, CreatureDTDR>;

static __forceinline ActorDTDR GetCreatureDTDR(Actor* actor_ref) {
    return ActorDTDR{ CreatureDTDR { DTDR{ actor_ref->avOwner.GetActorValue(76),actor_ref->avOwner.GetActorValue(18) } } };
}

static __forceinline ActorDTDR GetCharacterDTDR(Actor* actor_ref) {
    return ActorDTDR{ CharacterDTDR{ GetEqArmorDTDR(actor_ref),GetEqHelmetDTDR(actor_ref) } };
}

static EquipData FindEquipped(TESObjectREFR* thisObj, FormMatcher& matcher) {
    ExtraContainerChanges* pContainerChanges = static_cast<ExtraContainerChanges*>(thisObj->extraDataList.GetByType(kExtraData_ContainerChanges));
    return (pContainerChanges) ? pContainerChanges->FindEquipped(matcher) : EquipData();
}

static TESForm* GetEquippedObject(Actor* _actor,UINT32 _slot_index) {
    MatchBySlot matcher(_slot_index);
    EquipData equipD = FindEquipped(_actor, matcher);
    TESForm* pFound = equipD.pForm;
    if (pFound) return pFound;
    return nullptr;
}

static TESObjectARMO* GetEquippedArmorObj(Actor* _actor) {
    if (auto* eq_form = GetEquippedObject(_actor,BodyPart(UpperBody))){
        if (IS_TYPE(eq_form, TESObjectARMO)) {
            return static_cast<TESObjectARMO*>(eq_form);
        }
    }
    return nullptr;
}

static TESObjectARMO* GetEquippedHelmetObj(Actor* _actor) {
    if (auto* eq_form = GetEquippedObject(_actor, BodyPart(Headband))) {
        if (IS_TYPE(eq_form, TESObjectARMO)) {
            return static_cast<TESObjectARMO*>(eq_form);
        }
    }
    return nullptr;
}

static bool IsUnarmOrWeaponIsNull(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (!eq_form)
        return true;
    if (IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        //gLog.FormattedMessage("%d", _weap->eWeaponType);
        return _weap->eWeaponType == TESObjectWEAP::EWeaponType::kWeapType_HandToHandMelee;
    }
    return false;
}

static bool IsEquipThrowableMelee(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType == TESObjectWEAP::EWeaponType::kWeapType_OneHandThrown);
    }
    return false;
}

static bool IsEquipMelee(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType >= TESObjectWEAP::EWeaponType::kWeapType_HandToHandMelee &&
            _weap->eWeaponType <= TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee);
    }
    return false;
}

static bool IsEquipRanged(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType >= TESObjectWEAP::EWeaponType::kWeapType_OneHandPistol &&
            _weap->eWeaponType <= TESObjectWEAP::EWeaponType::kWeapType_TwoHandLauncher);
    }
    return false;
}

static bool IsEquipNotMelee(Actor* _actor) {
    auto* eq_form = GetEquippedObject(_actor, BodyPart(Weapon));
    if (eq_form && IS_TYPE(eq_form, TESObjectWEAP)) {
        auto* _weap = static_cast<TESObjectWEAP*>(eq_form);
        return (_weap->eWeaponType > TESObjectWEAP::EWeaponType::kWeapType_TwoHandMelee &&
            _weap->eWeaponType != TESObjectWEAP::EWeaponType::kWeapType_Last);
    }
    return false;
}

static auto GetArmorClassInfo(TESObjectARMO* _armor) {
    struct ArmorClassInfo {
        bool isPowerArmor;
        ArmorClass arClass;
    };
    TESBipedModelForm* biped = &_armor->bipedModel;
    if (!biped) return ArmorClassInfo{ false, ArmorClass::None };
    return ArmorClassInfo{ 
        biped->IsPowerArmor(),
        ((biped->bipedFlags & 0x80) ? ArmorClass::Heavy :
            ((biped->bipedFlags & 8) ? ArmorClass::Medium : ArmorClass::Light))
            };
}

static __forceinline NiNode* PCGet1stBlock(const char* _nodeName) {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson())
        return nullptr;

    if (NiNode* _1stNode = _pc->node1stPerson) {
        return _1stNode->GetBlock(_nodeName)->GetNiNode();
    }
}

static __forceinline NiNode* PCGet1stIronSightNode() {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson())
        return nullptr;

    if (NiNode* _isNode = _pc->ironSightNode) {
        return _isNode;
    }
    else if (NiNode* _1stNode = _pc->node1stPerson) {
        if (NiNode* _isNode = CdeclCall<NiNode*>(cdecl_getSubNodeByNameAddr, _1stNode, "##SightingNode")){
            return _isNode;
        }
    }
    return nullptr;
}

static __forceinline NiNode* GetPC1stPJNodeAlt() {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson()) {
        //gLog.Message("PC IS 3rd");
        return nullptr;
    }
    
   
    if (NiNode* _1stNode = _pc->node1stPerson) {
        if(NiAVObject* _pjNode = _1stNode->GetBlock("ProjectileNode"))
        {
            return _pjNode->GetNiNode();
        }
    }
    return nullptr;
}

static __forceinline NiNode* GetPC1stPJNode() {
    PlayerCharacter* _pc = PlayerCharacter::GetSingleton();
    if (_pc->IsThirdPerson()){
        return nullptr;
    }
    if (NiNode* _1stPjNode = GetActorPJNode(_pc)){
        return _1stPjNode;
    }
    
    if (NiNode* _1stNode = _pc->node1stPerson) {
        if (NiAVObject* _pjNode = _1stNode->GetBlock("ProjectileNode")) {
            return _pjNode->GetNiNode();
        }
        else if (NiNode* _1stPjNode = CdeclCall<NiNode*>(cdecl_getSubNodeByNameAddr, _1stNode, "ProjectileNode")) {
            return _1stPjNode;
        }
    }

   /* if (NiNode* _1stNode = _pc->node1stPerson) {
        NiNode* _pjNode =  _1stNode->GetBlock("ProjectileNode")->GetNiNode();
        if (_pjNode)
        {
            return _pjNode;
        }
    }*/
    return nullptr;
}


static __forceinline NiVector3 GetActorPJNodePos(Actor* _actor) {
    if (auto* pj_node = GetActorPJNode(_actor)) {
        return pj_node->m_transformWorld.translate;
    }
    return { 0,0,0 };
}

static __forceinline bool PJIsExplosion(const BGSProjectile* basepj) {
	return (basepj->projFlags & 2) != 0 && (basepj->type & 1) != 0;
}

static __forceinline float CalcProjSpeed(const BGSProjectile* basepj, const Projectile* _pjref) {
	return (basepj->speed * _pjref->speedMult);
}

static __forceinline SceneGraph* GetSceneGraph() {
	return (*(SceneGraph**)0x11DEB7C);
}

static __forceinline float CalcPosSquareDis(const NiVector3& posA, const NiVector3& posB) {
    return (Ut_Square((posA.x - posB.x)) + Ut_Square((posA.y - posB.y)) + Ut_Square((posA.z - posB.z)));
}

static __forceinline void CalcMidPoint3(const NiVector3& posA,const NiVector3& posB,NiVector3& out_mid) {
    out_mid.x = (posA.x + posB.x) / 2;
    out_mid.y = (posA.y + posB.y) / 2;
    out_mid.z = (posA.z + posB.z) / 2;
}

static __forceinline NiVector3 ScalarSub(const NiVector3& _from, const NiVector3& _to) {
    return {_to.x - _from.x,
            _to.y - _from.y,
            _to.z - _from.z};
}

static __forceinline NiVector3 ScalarAdd(const NiVector3& _v1, const NiVector3& _v2) {
    return { _v1.x + _v2.x,
            _v1.y + _v2.y,
            _v1.z + _v2.z };
}

static __forceinline float GetRefDistanceSquare(const TESObjectREFR* _refA, const TESObjectREFR* _refB) {
	return (Ut_Square(_refB->posX - _refA->posX) + Ut_Square(_refB->posY - _refA->posY) + Ut_Square(_refB->posZ - _refA->posZ));
}

static __forceinline float GetRefDistanceSquare2D(const TESObjectREFR* _refA, const TESObjectREFR* _refB) {
	return (Ut_Square(_refB->posX - _refA->posX) + Ut_Square(_refB->posY - _refA->posY));
}

static __forceinline bool IsPJHitscan(const BGSProjectile* _baseproj) {
	return (_baseproj->projFlags & BGSProjectile::kFlags_Hitscan) != 0;
}

static __forceinline float GetPJMuzzleFlashDuration(const BGSProjectile* _baseproj) {
	return _baseproj->flashDuration;
}

static __forceinline bool IsPJHitscan(const Projectile* _proj) {
	return (((BGSProjectile*)_proj->baseForm)->projFlags & BGSProjectile::kFlags_Hitscan) != 0;
}

static __forceinline BGSImpactDataSet* GetImpactDataSet(const TESObjectWEAP* _weap) {
	return _weap->impactDataSet;
}

enum CreatureType : UInt8 {
    NotCreature = 8,
    Animal = 0,
    Mutated_Animal = 1,
    Mutated_Insect = 2,
    Abomination = 3,	// Intellective
    Supermutant = 4,	// Intellective
    Feral_Ghoul = 5,
    Robot = 6,			// Intellective
    Giant = 7
};

enum Ret_CreatureType : UInt8 {
    Ret_Creature = 0,
    Ret_IntellectiveCreature = 1,
    Ret_NotCreature = 2
};

static CreatureType __forceinline GetCreatureType(const Actor* _actor) {
    if (const TESCreature* creature = (TESCreature*)(_actor->GetActorBase())) {
        if IS_ID(creature, TESCreature) {
            return (CreatureType)creature->type;
        }
    }
    return NotCreature;
}

static bool __forceinline IsIntellective(const Actor* _actor) {
    CreatureType c_type = GetCreatureType(_actor);
    return (c_type == NotCreature || c_type == Abomination || c_type == Supermutant || c_type == Robot);
}

static bool __forceinline IsGunUser(const Actor* _actor) {
    CreatureType c_type = GetCreatureType(_actor);
    return (c_type == NotCreature  || c_type == Supermutant || c_type == Robot);
}

static bool __forceinline IsRobot(const Actor* _actor) {
    return GetCreatureType(_actor) == Robot;
}

enum MaterialType
{
    kMaterial_Stone,
    kMaterial_Dirt,
    kMaterial_Grass,
    kMaterial_Glass,
    kMaterial_Metal,
    kMaterial_Wood,
    kMaterial_Organic,
    kMaterial_Cloth,
    kMaterial_Water,
    kMaterial_HollowMetal,
    kMaterial_OrganicBug,
    kMaterial_OrganicGlow
};

alignas(16) const UInt8 kMaterialConvert[] =
{
    kMaterial_Stone,
    kMaterial_Cloth,
    kMaterial_Dirt,
    kMaterial_Glass,
    kMaterial_Grass,
    kMaterial_Metal,
    kMaterial_Organic,
    kMaterial_Organic,
    kMaterial_Water,
    kMaterial_Wood,
    kMaterial_Stone,
    kMaterial_Metal,
    kMaterial_Wood,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_HollowMetal,
    kMaterial_HollowMetal,
    kMaterial_Dirt,
    kMaterial_Stone,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_HollowMetal,
    kMaterial_HollowMetal,
    kMaterial_Glass,
    kMaterial_HollowMetal,
    kMaterial_Metal,
    kMaterial_Metal,
    kMaterial_HollowMetal,
    kMaterial_HollowMetal,
    kMaterial_Wood,
    kMaterial_Wood
};

// Deal with the material type from the 'raw' hit material which is recorded in impact data
static __forceinline UINT8 MaterialConverter(UINT32 hitmaterial) {
    return (hitmaterial <= 31 ? kMaterialConvert[hitmaterial] : (UINT8)255);
}

// arg - hit material which have been converted
static __forceinline UINT8 IsMetal(UINT32 material) {
    return material == 4 || material == 9;
}

/*
 ==== material ====
0	Stone | 1	Dirt | 2	Grass | 3	Glass
4	Metal | 5	Wood | 6  Organic | 7	Cloth
8	Water | 9	Hollow Metal | 10	Organic Bug | 11	Organic Glow
*/
static __forceinline BGSImpactData* GetImpactDataByMaterial(const TESObjectWEAP* _weap,UINT8 _material) {
	if (_material < 12) return GetImpactDataSet(_weap)->impactDatas[_material];
	return nullptr;
}

static __forceinline void ResetRefCollision(TESObjectREFR* _ref) {
    std::vector<NiNode*> col_nodes;
    GetCollisionNodes(_ref, OUT col_nodes);
    for (NiNode* nd : col_nodes)
    {
        ResetNodeCollision(nd);
    }
}

static __forceinline void InfoRefCollision(TESObjectREFR* _ref) {
    std::vector<NiNode*> col_nodes;
    gLog.FormattedMessage("render state flags %u",_ref->renderState->flags);
    GetCollisionNodes(_ref, OUT col_nodes);
    for (NiNode* nd : col_nodes)
    {
        gLog.FormattedMessage("col flag %u", nd->m_collisionObject->flags);
        PrintWorldObjInfo(nd);
    }
}

static __forceinline void FindRefInMovableObjList(TESObjectREFR* _ref) {
    auto& obj_list = ProcessManager::Get()->objects;
    for (auto iter = obj_list.Begin(); iter; ++iter){
        if (*iter && (*iter)->refID == _ref->refID) {
            gLog.FormattedMessage("Find _ref %x", _ref->refID);
            return;
        }
    }
    gLog.FormattedMessage("Cant Find _ref %x", _ref->refID);
}