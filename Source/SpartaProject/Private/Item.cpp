// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

DEFINE_LOG_CATEGORY(LogSparta);


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Scene Component를 생성하고 루트로 설정
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Static Mesh Component를 생성하고 Scene Component에 Attach
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(SceneRoot);

	// Static Mesh를 코드에서 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Resources/Props/SM_Chair.SM_Chair"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

	// Material을 코드에서 설정
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Resources/Materials/M_Metal_Gold.M_Metal_Gold"));
	if (MaterialAsset.Succeeded())
	{
		StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
	}
	UE_LOG(LogSparta, Warning, TEXT("%s Constructor"), *GetName());
	RotationSpeed = 90.0f;
	LocationSpeed = 90.0f;
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogSparta, Warning, TEXT("%s PostInitializeComponents"), *GetName());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogSparta, Warning, TEXT("%s BeginPlay"), *GetName());

	SetActorLocation(FVector(300.0f, 200.0f, 100.0f));
	SetActorRotation(FRotator(0.0f, 90.0f, 0.0f));
	SetActorScale3D(FVector(2.0f));

	/*FVector NewLocation(300.0f, 200.0f, 100.0f);
	FRotator NewRotation(0.0f, 90.0f, 0.0f);
	FVector NewScale(2.0f);

	FTransform NewTransform(NewRotation, NewLocation, NewScale);

	SetActorTransform(NewTransform);*/

	OnItemPickedUP();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FMath::IsNearlyZero(RotationSpeed))
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
	}

	if (!FMath::IsNearlyZero(LocationSpeed))
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
		AddActorLocalOffset(FVector(0.0f, 0.0f, LocationSpeed * DeltaTime));
	}
}

void AItem::Destroyed()
{
	UE_LOG(LogSparta, Warning, TEXT("%s Destroyed"), *GetName());

	Super::Destroyed();
}

void AItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UE_LOG(LogSparta, Warning, TEXT("%s EndPlay"), *GetName());

	Super::EndPlay(EndPlayReason);
}

void AItem::ResetActorPosition()
{
	SetActorLocation(FVector::ZeroVector);
}

float AItem::GetRotationSpeed() const
{
	return RotationSpeed;
}