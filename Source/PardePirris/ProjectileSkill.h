// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseSkill.h"                 
#include "ProjectileSkill.generated.h"

class ASkillProjectile;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class PARDEPIRRIS_API UProjectileSkill : public UBaseSkill
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TSubclassOf<ASkillProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SpawnOffset = 100.f;

	virtual void Activate(class AMainCharacter* Caster) override;
};