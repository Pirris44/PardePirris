// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseSkill.generated.h"

// Forward declaration
class AMainCharacter;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
/**
 * 
 */
class PARDEPIRRIS_API UBaseSkill : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category ="Skill")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float CooldownTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UAnimMontage* SkillMontage;

	// --- Estado interno ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill")
	bool bIsOnCooldown = false;

	// --- Funciones ---
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void Activate(AMainCharacter* Caster);
protected:
	void StartCooldown(AMainCharacter* Caster);
	void FinishCooldown();

	FTimerHandle CooldownTimerHandle;
};
