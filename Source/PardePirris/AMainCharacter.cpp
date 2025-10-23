// Fill out your copyright notice in the Description page of Project Settings.


#include "AMainCharacter.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/Controller.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>

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
