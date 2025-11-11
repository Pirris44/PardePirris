#include "ProjectileSkill.h"
#include "AMainCharacter.h"
#include "SkillProjectile.h"
#include "Kismet/GameplayStatics.h"

void UProjectileSkill::Activate(AMainCharacter* Caster)
{
	if (!Caster || bIsOnCooldown || !ProjectileClass) return;

	// Animación
	if (SkillMontage)
	{
		Caster->PlayAnimMontage(SkillMontage);
	}


	FVector ForwardVector = Caster->GetActorForwardVector();
	FVector SpawnLocation = Caster->GetActorLocation() + (ForwardVector * 50.f) + FVector(0.f, 0.f, 50.f);
	FRotator SpawnRotation = Caster->GetActorRotation();

	FActorSpawnParameters Params;
	Params.Owner = Caster;
	Params.Instigator = Caster;

	// Spawn del proyectil
	ASkillProjectile* Projectile = Caster->GetWorld()->SpawnActor<ASkillProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, Params);
	if (Projectile)
	{
		Projectile-> Damage = this->Damage;
	}

	// Iniciar cooldown
	StartCooldown(Caster);
}