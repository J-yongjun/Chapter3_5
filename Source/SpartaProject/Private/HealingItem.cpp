// HealingItem.cpp

#include "HealingItem.h"
#include "SpartaCharacter.h"

AHealingItem::AHealingItem()
{
	HealAmount = 20.0f;
	ItemType = "Healing";
}

void AHealingItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        // 회복 디버그 메시지
        /*GEngine->AddOnScreenDebugMessage(
            -1, 
            2.0f, 
            FColor::Green, FString::Printf(TEXT("Player gained %d HP!"), HealAmount));
        */
        if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
        {
            PlayerCharacter->AddHealth(HealAmount);
        }

        DestroyItem();
    }
}