// SpawnVolume.cpp

#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트를 생성하고, 이 액터의 루트로 설정
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

    ItemDataTable = nullptr;
}

AActor* ASpawnVolume::SpawnRandomItem()
{
    if (FItemSpawnRow* SelectedRow = GetRandomItem())
    {
        if (UClass* ActualClass = SelectedRow->ItemClass.Get())
        {
            return SpawnItem(ActualClass);
        }
    }

    return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
    // 1) 박스 컴포넌트의 스케일된 Extent, 즉 x/y/z 방향으로 반지름(절반 길이)을 구함
    FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
    // 2) 박스 중심 위치
    FVector BoxOrigin = SpawningBox->GetComponentLocation();

    // 3) 각 축별로 -Extent ~ +Extent 범위의 무작위 값 생성
    return BoxOrigin + FVector(
        FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
        FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
        FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
    );
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
    if (!ItemDataTable) return nullptr;

    // 1) 모든 Row(행) 가져오기
    TArray<FItemSpawnRow*> AllRows;
    static const FString ContextString(TEXT("ItemSpawnContext"));
    ItemDataTable->GetAllRows(ContextString, AllRows);

    if (AllRows.IsEmpty()) return nullptr;

    // 2) 전체 확률 합 구하기
    float TotalChance = 0.0f; // 초기화
    for (const FItemSpawnRow* Row : AllRows) // AllRows 배열의 각 Row를 순회
    {
        if (Row) // Row가 유효한지 확인
        {
            TotalChance += Row->SpawnChance; // SpawnChance 값을 TotalChance에 더하기
        }
    }

    // 3) 0 ~ TotalChance 사이 랜덤 값
    const float RandValue = FMath::FRandRange(0.0f, TotalChance);
    float AccumulateChance = 0.0f;

    // 4) 누적 확률로 아이템 선택
    for (FItemSpawnRow* Row : AllRows)
    {
        AccumulateChance += Row->SpawnChance;
        if (RandValue <= AccumulateChance)
        {
            return Row;
        }
    }

    return nullptr;
}
AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
    if (!ItemClass) return nullptr;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ItemClass,
        GetRandomPointInVolume(),
        FRotator::ZeroRotator
    );

    return SpawnedActor;
}