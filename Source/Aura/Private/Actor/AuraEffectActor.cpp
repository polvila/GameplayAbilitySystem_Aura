// Copyright Take Away Games


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC)) return;

	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle =
		TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	if (!EffectSpecHandle.IsValid()) return;
	const FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();
	TargetASC->ApplyGameplayEffectSpecToSelf(*Spec);
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	for (const auto& GameplayEffectItem : GameplayEffects)
	{
		if (GameplayEffectItem.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectItem.GameplayEffectClass);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	for (const auto& GameplayEffectItem : GameplayEffects)
	{
		if (GameplayEffectItem.EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectItem.GameplayEffectClass);
		}

		RemoveInfiniteEffectByPolicy(TargetActor, GameplayEffectItem);
	}
}

void AAuraEffectActor::RemoveInfiniteEffectByPolicy(AActor* TargetActor, const FGameplayEffectItem& GameplayEffectItem)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC)) return;

	FGameplayEffectQuery Query;
	Query.CustomMatchDelegate.BindLambda([&](const FActiveGameplayEffect& Effect)
	{
		const UGameplayEffect* EffectDefinition = Effect.Spec.Def.Get();
		return IsValid(EffectDefinition) &&
			   GameplayEffectItem.InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap &&
			   EffectDefinition->GetClass() == GameplayEffectItem.GameplayEffectClass &&
			   EffectDefinition->DurationPolicy == EGameplayEffectDurationType::Infinite;
	});

	TargetASC->RemoveActiveEffects(Query, 1);
}