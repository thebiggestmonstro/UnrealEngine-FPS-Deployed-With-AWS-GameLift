// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Career/ShooterCareerPage.h"
#include "Components/ScrollBox.h"
#include "Data/SpecialElimData.h"
#include "ShooterTypes/ShooterTypes.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "UI/Portal/Dashboard/CareerAchievement.h"

void UShooterCareerPage::OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse)
{
	Super::OnRetrieveMatchStats(RetrieveMatchStatsResponse);

	if (!ScrollBox_Achievements || !SpecialElimData || !CareerAchievementClass)
	{
		return;
	}

	ScrollBox_Achievements->ClearChildren();

	TMap<ESpecialElimType, int32> AchievementData;

	if (RetrieveMatchStatsResponse.hits > 0) AchievementData.Emplace(ESpecialElimType::Hits, RetrieveMatchStatsResponse.hits);
	if (RetrieveMatchStatsResponse.misses > 0) AchievementData.Emplace(ESpecialElimType::Misses, RetrieveMatchStatsResponse.misses);
	if (RetrieveMatchStatsResponse.scoredElims > 0) AchievementData.Emplace(ESpecialElimType::ScoredElims, RetrieveMatchStatsResponse.scoredElims);
	if (RetrieveMatchStatsResponse.defeats > 0) AchievementData.Emplace(ESpecialElimType::Defeats, RetrieveMatchStatsResponse.defeats);
	if (RetrieveMatchStatsResponse.highestStreak > 0) AchievementData.Emplace(ESpecialElimType::Streak, RetrieveMatchStatsResponse.highestStreak);
	if (RetrieveMatchStatsResponse.dethroneElims > 0) AchievementData.Emplace(ESpecialElimType::Dethrone, RetrieveMatchStatsResponse.dethroneElims);
	if (RetrieveMatchStatsResponse.gotFirstBlood > 0) AchievementData.Emplace(ESpecialElimType::FirstBlood, RetrieveMatchStatsResponse.gotFirstBlood);
	if (RetrieveMatchStatsResponse.revengeElims > 0) AchievementData.Emplace(ESpecialElimType::Revenge, RetrieveMatchStatsResponse.revengeElims);
	if (RetrieveMatchStatsResponse.showstopperElims > 0) AchievementData.Emplace(ESpecialElimType::Showstopper, RetrieveMatchStatsResponse.showstopperElims);
	if (RetrieveMatchStatsResponse.headShotElims > 0) AchievementData.Emplace(ESpecialElimType::Headshot, RetrieveMatchStatsResponse.headShotElims);

	for (const TPair<ESpecialElimType, int32>& Pair : AchievementData)
	{
		const FSpecialElimInfo* ElimInfo = SpecialElimData->SpecialElimInfo.Find(Pair.Key);
		
		check(ElimInfo);

		UCareerAchievement* CareerAchievement = CreateWidget<UCareerAchievement>(this, CareerAchievementClass);
		if (IsValid(CareerAchievement))
		{
			CareerAchievement->SetAchievementText(ElimInfo->CareerPageAchievementName, Pair.Value);
			if (ElimInfo->ElimIcon)
			{
				CareerAchievement->SetAchievementIcon(ElimInfo->ElimIcon);
			}
			ScrollBox_Achievements->AddChild(CareerAchievement);
		}
	}
}



