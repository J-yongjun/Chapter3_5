// ItemSpawnRow.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemSpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FItemSpawnRow : public FTableRowBase
{
	GENERATED_BODY()

public:
    // 아이템 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemName;
    // 어떤 아이템 클래스를 스폰할지
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> ItemClass;
    // 이 아이템의 스폰 확률
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnChance;
};
