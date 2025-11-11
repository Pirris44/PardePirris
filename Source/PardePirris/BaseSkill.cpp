// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSkill.h"
#include "AMainCharacter.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"


void UBaseSkill:: Activate(AMainCharacter* Caster)
{

	if (!Caster || bIsOnCooldown)
		return;

	// Reproducir animación si existe
	if (SkillMontage)
	{
		Caster->PlayAnimMontage(SkillMontage);
	}

	// Aplicar daño simple al frente del personaje
	FHitResult Hit;
	FVector Start = Caster->GetActorLocation();
	FVector End = Start + Caster->GetActorForwardVector() * 300.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Caster);
	if (Caster->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != Caster)
		{
			UGameplayStatics::ApplyDamage(HitActor, Damage, Caster->GetController(), Caster, nullptr);
		}
	}

	// Inicia el cooldown
	StartCooldown(Caster);
}
void UBaseSkill::StartCooldown(AMainCharacter* Caster)
{
	if (!Caster) return;
	bIsOnCooldown = true;

	
	Caster->GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &UBaseSkill::FinishCooldown, CooldownTime, false);
}
void UBaseSkill::FinishCooldown()
{
	bIsOnCooldown = false;
}