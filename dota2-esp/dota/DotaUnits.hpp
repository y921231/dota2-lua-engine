#pragma once

#include "..\utils\global_address_retriever.hpp"
#include "DotaParticle.hpp"
#include "DotaModifierManager.hpp"

class CDotaItem : public CBaseEntity {
public:
  const char* GetName() {
    int offset = 0x778;
    return *(const char**)(*(int*)(this + offset));
  }
  int GetItemId() {
    int offset = 0x778;
    return *(int*)((*(int*)(this + offset)) + 0x3c);
  }
  bool GetRequiresCharges() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_Item","m_bRequiresCharges");
    return *(bool*)(this + offset);
  }
  int GetCurrentCharges() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_Item","m_iCurrentCharges");
    return *(int*)(this + offset);
  }
  int GetInitialCharges() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_Item","m_iInitialCharges");
    return *(int*)(this + offset);
  }
  int GetPurchaser() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_Item","m_hPurchaser");
    return *(int*)(this + offset);
  }
  float GetPurchaseTime() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_Item","m_flPurchaseTime");
    return *(float*)(this + offset);
  }
};

class CUnitInventory {
public:
  CDotaItem* GetItemInSlot(int index) {
    int offset = 0x18;
    int handle = *(int*)(this + offset + index * 4);
    return (CDotaItem*)GlobalInstanceManager::GetClientEntityList()->GetClientEntityFromHandle(handle);
  }
};

class CBaseNpc : public CBaseEntity {
public:
  CParticleProperty* GetParticleProp() {
    PVOID pParticleProp = (PVOID)(this + 0x10);
    typedef CParticleProperty* ( __thiscall* OriginalFn )( PVOID );
    return utils::GetVtableFunction<OriginalFn>( pParticleProp, 0 )( pParticleProp );
  }
  float GetEffectiveInvisibilityLevel() {
    typedef float ( __thiscall* OriginalFn )( PVOID );
    return utils::GetVtableFunction<OriginalFn>(this, 277)(this);
  }
  bool IsVisibleByEnemyTeam() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_iTaggedAsVisibleByTeam");
    return *(byte*)(this + offset) == 30;
  }
  int GetHealth() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_iHealth");
    return *(int*)(this + offset);
  }
  float GetVisibilityLevel() {
    int offset = 0x3160;
    return *(float*)(this + offset);
  }
  void SetVisibilityLevel(float level) {
    int offset = 0x3160;
    *(float*)(this + offset) = level;
    *(float*)(this + 0x118C) = level > 0 ? 1 : 0;
    GetEffectiveInvisibilityLevel();
  }
  float GetMana() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_flMana");
    return *(float*)(this + offset);
  }
  float GetMaxMana() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_flMaxMana");
    return *(float*)(this + offset);
  }
  CUnitInventory* GetInventory() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_Inventory");
    return (CUnitInventory*)(this + offset);
  }
  CModifierManager* GetModifierManager() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_ModifierManager");
    return (CModifierManager*)(this + offset);
  }
  float GetPhysicalArmor() {
    typedef float ( __thiscall* OriginalFn )( PVOID );
    return utils::GetVtableFunction<OriginalFn>(this, 261)(this);
  }

  bool IsEntityLastHittable(CBaseNpc* entity) {
    int health = entity->GetHealth();
    if (health <= 0) return false;
    int damage = GetDamageMin() + GetBonusDamage();
    float reduced_damage = damage - ((damage * entity->GetPhysicalArmorReduction()) / 100);
    return health / damage < 1;
  }

  float GetPhysicalArmorReduction() {
    typedef float ( __thiscall* OriginalFn )( PVOID );
    float armor = utils::GetVtableFunction<OriginalFn>(this, 261)(this);
    if (armor <= 0) return 0;
    return ((0.06 * armor) / (1 + 0.06 * armor)) * 100;
  }
  int GetDamageMin() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_iDamageMin");
    return *(int*)(this + offset);
  }
  int GetDamageMax() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC","m_iDamageMax");
    return *(int*)(this + offset);
  }
  int GetBonusDamage() {
    typedef int ( __thiscall* OriginalFn )( PVOID );
    return utils::GetVtableFunction<OriginalFn>(this, 283)(this);
  }
  float GetAttackRange() {
    uint32_t address = GlobalAddressRetriever::GetInstance().GetStaticAddress("BaseNpc::GetAttackRange");

    float range;

    __asm {
      mov esi, this
        call get_attack_range_address_
        movss [range], xmm0
    }

    return range;
  }
  bool IsEntityInRange(CBaseEntity* base_enity, float distance) {
    uint32_t address = GlobalAddressRetriever::GetInstance().GetStaticAddress("BaseNpc::IsEntityInRange");

    bool in_range;

    __asm {
      push distance
        mov esi, base_enity
        mov ecx, this
        call is_entity_in_range_address_
        mov in_range, al
    }

    return in_range;
  }
private:
  static unsigned long is_entity_in_range_address_;
  static unsigned long get_attack_range_address_;
};

class UnitAnnouncer : public CBaseNpc {
public:
  void SetAnnouncerItem(EconItemView* item, bool unknown0) {
    uint32_t address = GlobalAddressRetriever::GetInstance().GetStaticAddress("UnitAnnouncer::SetAnnouncerItem");

    __asm {
      mov ecx, item
        mov esi, this
        mov al, unknown0
        call set_announcer_item_address_
    }
  }
};

class CBaseNpcHero : public CBaseNpc {
 public:
  bool IsIllusion() {
    int offset = sourcesdk::NetVarManager::GetInstance().GetNetVarOffset("DT_DOTA_BaseNPC_Hero","m_hReplicatingOtherHeroModel");
    return *(int*)(this + offset) != -1;
  }
};