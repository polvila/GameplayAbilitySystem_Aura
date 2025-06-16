// Copyright Take Away Games


#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& EffectSpec,
                                                FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer EffectTags;
	EffectSpec.GetAllAssetTags(EffectTags);
	for (const auto& Tag : EffectTags)
	{
		//TODO: Broadcast the tag to the Widget Controller
		const FString TagString = Tag.ToString();
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, TagString);
	}
}
