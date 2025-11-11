// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseSkill.h"
#include "SkillProjectile.h"
#include "AMainCharacter.generated.h"




class UInputMappingContext;
class UInputAction;

UCLASS()
class PARDEPIRRIS_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//MOVEMENT FUNCTIONS

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveForwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveBackwardAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveRightAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveLeftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpActionInput;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Skill1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Skill2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Skill3Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Skill4Action;

	void MoveForward(const FInputActionValue& Value);
	void MoveBackward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void MoveLeft(const FInputActionValue& Value);
	void JumpAction(const FInputActionValue& Value);

	UFUNCTION()
	void Skill1();
	void StartSkill1Targeting();
	void ExecuteSkill1();
	UFUNCTION()
	void Skill2();
	UFUNCTION()
	void Skill3();
	UFUNCTION()
	void Skill4();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MinCapCameraPitch = -80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MaxCapCameraPitch = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivity = 1.0f; // Ajusta según prefieras


	UPROPERTY(EditDefaultsOnly, Category = "Abilities|Prediction")
	TSubclassOf<ASkillProjectile> PredictionProjectileClass;

	// Componente visual para dibujar la trayectoria 
	
	// El índice de la habilidad que estamos apuntando (si es -1, no estamos apuntando)
	int32 ActiveSkillIndex = -1;

	void UpdateSkillPrediction();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//SISTEMA DE VIDA Y MUERTE

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Health")
	float MaxHealth = 100.f;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Die();

	//SISTEMA DE TARGETING DE HABILIDADES
	void ToggleSkillTargeting(int32 SkillIndex, bool bIsTargeting);

	//SISTEMA DE HABILIDADES

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Skills")
	TArray<UBaseSkill*> Skills;

	UFUNCTION(BlueprintCallable, Category = "Skills")
	void UseSkill(int32 Index);

	//SISTEMA DE DAÑOS DE LAS HABILIDADES

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Damage")
	float Skill1Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Damage")
	float Skill2Damage = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Damage")
	float Skill3Damage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skills|Damage")
	float Skill4Damage = 70.f;

	

	
private:
	

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	//HACER DAÑO AL ENEMIGO

	void ApplyDamageToEnemy(float Damage);
};
