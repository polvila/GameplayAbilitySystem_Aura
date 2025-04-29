// Copyright Take Away Games


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(10.f);
	InitMaxMana(50.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
                                            FGameplayEffectContextHandle& EffectContextHandle,
                                            FEffectProperties& SourceProps, FEffectProperties& TargetProps) const
{
	EffectContextHandle = Data.EffectSpec.GetContext();
	SourceProps.ASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(SourceProps.ASC) && SourceProps.ASC->AbilityActorInfo.IsValid() && SourceProps.ASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceProps.AvatarActor = SourceProps.ASC->AbilityActorInfo->AvatarActor.Get();
		SourceProps.Controller = SourceProps.ASC->AbilityActorInfo->PlayerController.Get();
		if (!IsValid(SourceProps.Controller) && IsValid(SourceProps.AvatarActor))
		{
			if (const APawn* Pawn = Cast<APawn>(SourceProps.AvatarActor))
			{
				SourceProps.Controller = Pawn->GetController();
			}
		}
		if (IsValid(SourceProps.Controller))
		{
			SourceProps.Character = SourceProps.Controller->GetCharacter();
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetProps.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetProps.Controller = Data.Target.AbilityActorInfo->PlayerController.Get();

		if (IsValid(TargetProps.Controller))
		{
			TargetProps.Character = TargetProps.Controller->GetCharacter();
		}

		TargetProps.ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetProps.AvatarActor);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle EffectContextHandle;
	FEffectProperties SourceProps;
	FEffectProperties TargetProps;

	SetEffectProperties(Data, EffectContextHandle, SourceProps, TargetProps);

	//TODO: Use the effect properties stored?
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}