// BigCoinItem.h

#include "BigCoinItem.h"

ABigCoinItem::ABigCoinItem()
{
	PointValue = 50;
	ItemType = "BigCoin";
}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
	// 부모의 기본 점수 획득 로직 사용
	Super::ActivateItem(Activator);

	// 빅 코인만의 추가 동작(이펙트, 사운드 재생 등)을 여기서 추가할 수 있음
}