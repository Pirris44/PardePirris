// Fill out your copyright notice in the Description page of Project Settings.


#include "AMainCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/Controller.h>
#include "SkillProjectile.h"
#include "ProjectileSkill.h"
#include "BaseSkill.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "Components/SphereComponent.h"
#include <GameFramework/ProjectileMovementComponent.h>




// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;           
	SpringArm->bInheritYaw = true;              
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f)); // Ángulo inicial
	

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	//Esto hace que al empezar sea el Player0 el que controle al Personaje
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Movimiento
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

    
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  
	if (ActiveSkillIndex != -1)
	{
		UpdateSkillPrediction();
	}
}

void AMainCharacter::TakeDamage(float DamageAmount)
{
}

void AMainCharacter::Heal(float HealAmount)
{
}

void AMainCharacter::Die()
{
}


// Habilidad de apuntado y predicción

void AMainCharacter::UpdateSkillPrediction()
{
	if (!PredictionProjectileClass) return;

	// Obtener la clase de proyectil para obtener la velocidad
	const ASkillProjectile* DefaultProjectile = Cast<ASkillProjectile>(PredictionProjectileClass.GetDefaultObject());
	if (!DefaultProjectile || !DefaultProjectile->ProjectileMovement || !DefaultProjectile->CollisionComponent)
		return;

	FVector LaunchVelocity = GetActorForwardVector() * DefaultProjectile->ProjectileMovement->InitialSpeed;
	FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 50.f + FVector(0.f, 0.f, 50.f);

	FPredictProjectilePathParams Params;
	Params.StartLocation = StartLocation;
	Params.LaunchVelocity = LaunchVelocity;
	Params.ProjectileRadius = DefaultProjectile->CollisionComponent->GetScaledSphereRadius();
	Params.bTraceWithChannel = true;
	Params.OverrideGravityZ = 0.0f;
	Params.OverrideGravityZ = true;
	Params.TraceChannel = ECC_Visibility;
	Params.ActorsToIgnore.Add(this);
	Params.DrawDebugType = EDrawDebugTrace::ForDuration; // Dibujar línea para debug.
	
	FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(this, Params, Result);

	// Dibuja una línea roja si no usas un componente visual
	if (Result.PathData.Num() > 1)
	{
		for (int32 i = 0; i < Result.PathData.Num() - 1; ++i)
		{
			DrawDebugLine(
				GetWorld(),
				Result.PathData[i].Location,
				Result.PathData[i + 1].Location,
				FColor::Red,
				false,
				0.f,
				0,
				3.f
			);
		}
	}
}

void AMainCharacter::UseSkill(int32 Index)
{
	if(Skills.IsValidIndex(Index)&& Skills[Index])
	{
		Skills[Index]->Activate(this);
	}
}



void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AMainCharacter::MoveForward);
		EnhancedInput->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &AMainCharacter::MoveBackward);
		EnhancedInput->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AMainCharacter::MoveRight);
		EnhancedInput->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &AMainCharacter::MoveLeft);
		EnhancedInput->BindAction(JumpActionInput, ETriggerEvent::Triggered, this, &AMainCharacter::JumpAction);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);

		EnhancedInput->BindAction(Skill1Action, ETriggerEvent::Started, this, &AMainCharacter::StartSkill1Targeting);
		EnhancedInput->BindAction(Skill1Action, ETriggerEvent::Completed, this, &AMainCharacter::ExecuteSkill1);
		EnhancedInput->BindAction(Skill2Action, ETriggerEvent::Triggered, this, &AMainCharacter::Skill2);
		EnhancedInput->BindAction(Skill3Action, ETriggerEvent::Triggered, this, &AMainCharacter::Skill3);
		EnhancedInput->BindAction(Skill4Action, ETriggerEvent::Triggered, this, &AMainCharacter::Skill4);
	}
}
void AMainCharacter::MoveForward(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (Controller && AxisValue != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, AxisValue);
	}
}

void AMainCharacter::MoveBackward(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (Controller && AxisValue != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, -AxisValue); // se invierte para entendernos
	}
}

void AMainCharacter::MoveRight(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (Controller && AxisValue != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Right, AxisValue);
	}
}

void AMainCharacter::MoveLeft(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (Controller && AxisValue != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Right, -AxisValue); // se invierte
	}
}


void AMainCharacter::JumpAction(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
		Jump();
	else
		StopJumping();
}

void AMainCharacter::Skill1()
{
	UE_LOG(LogTemp, Warning, TEXT("Skill 1 pressed!"));
	UseSkill(0);
}
void AMainCharacter::StartSkill1Targeting()
{
	// Mostrar la línea de predicción para la habilidad 0
	ToggleSkillTargeting(0, true);
}

// Funcion que se enlaza al ETriggerEvent::Completed
void AMainCharacter::ExecuteSkill1()
{
	// Ocultar la línea de predicción
	ToggleSkillTargeting(0, false);

	// Ejecutar la habilidad
	UE_LOG(LogTemp, Warning, TEXT("Skill 1 executed!"));
	UseSkill(0);
}
void AMainCharacter::ToggleSkillTargeting(int32 SkillIndex, bool bIsTargeting)
{
	ActiveSkillIndex = bIsTargeting ? SkillIndex : -1;

	if (bIsTargeting)
	{
		// Si activamos, queremos actualizar la predicción cada frame
		PrimaryActorTick.bCanEverTick = true;
	}
	else
	{
		
	}
}
void AMainCharacter::Skill2()
{
}

void AMainCharacter::Skill3()
{
}

void AMainCharacter::Skill4()
{
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxis = Value.Get<FVector2D>();

	

	if (!Controller) return;

	// Rotación horizontal (yaw)
	AddControllerYawInput(LookAxis.X * MouseSensitivity);
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		FRotator ControlRotation = PC->GetControlRotation();

		float NewPitch = ControlRotation.Pitch - (LookAxis.Y * MouseSensitivity); // invertir si necesario
		NewPitch = FMath::ClampAngle(NewPitch, MinCapCameraPitch, MaxCapCameraPitch);

		ControlRotation.Pitch = NewPitch;
		PC->SetControlRotation(ControlRotation);
	}
	//AddControllerPitchInput(LookAxis.Y * MouseSensitivity);
}

void AMainCharacter::ApplyDamageToEnemy(float Damage)
{

}
