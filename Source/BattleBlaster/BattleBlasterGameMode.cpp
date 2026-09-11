// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleBlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Tower.h"
#include "BattleBlasterGameInstance.h"

void ABattleBlasterGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> Towers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(),ATower::StaticClass(),Towers);

    TowerCount = Towers.Num();
    UE_LOG(LogTemp,Display,TEXT("Number of towers: %d"),TowerCount);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
    if(PlayerPawn){
        Tank = Cast<ATank>(PlayerPawn);
        if(!Tank){
            UE_LOG(LogTemp,Display,TEXT("GameMode Failed to find the tank actor!"));
        }
    }

    int32 LoopIndex = 0;
    while(LoopIndex < TowerCount){
        AActor* TowerActor = Towers[LoopIndex];
        if(TowerActor){
            ATower* Tower = Cast<ATower>(TowerActor);
            if(Tower && Tank){
                Tower->Tank = Tank;
            }
        }
        LoopIndex++;
    }

    CountdownSeconds = CountdownDelay;
    GetWorldTimerManager().SetTimer(CountdownTimerHandle,this,&ABattleBlasterGameMode::OnCountdownTimerTimeout,1.0f,true);
}

void ABattleBlasterGameMode::OnCountdownTimerTimeout()
{
    CountdownSeconds -= 1;
    if(CountdownSeconds > 0){
        UE_LOG(LogTemp,Display,TEXT("Countdown: %d"),CountdownSeconds);

    }else if(CountdownSeconds == 0){
        UE_LOG(LogTemp,Display,TEXT("GO!"));
        Tank->SetPlayerEnabled(true);
    }else{
        GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
        UE_LOG(LogTemp,Display,TEXT("Clear Timer"));
    }
}

void ABattleBlasterGameMode::ActorDied(AActor *DeadActor)
{
    bool isGameOver = false;
    if(DeadActor == Tank){
        Tank->HandleDestruction();
        isGameOver = true;
    }else{
        ATower* DeadTower = Cast<ATower>(DeadActor);
        if(DeadTower){
            DeadTower->HandleDestruction();
            TowerCount--;
            if(TowerCount == 0){
                isGameOver = true;
                isVictory = true;
            }
        }
    }

    if(isGameOver){
        FString GameOverString = isVictory ? "Victory" : "Defeat";
        UE_LOG(LogTemp,Display,TEXT("Game over: %s"),*GameOverString);

        FTimerHandle GameOverTimerHandle;
        GetWorldTimerManager().SetTimer(GameOverTimerHandle,this,&ABattleBlasterGameMode::OnGameOverTimerTimeout,GameOverDelay,false);
    }
}

void ABattleBlasterGameMode::OnGameOverTimerTimeout()
{
    UGameInstance* GameInstance = GetGameInstance();
    if(GameInstance){
        UBattleBlasterGameInstance* BattleBlasterGameInstance = Cast<UBattleBlasterGameInstance>(GameInstance);
        if(BattleBlasterGameInstance){
            if(isVictory){
                BattleBlasterGameInstance->LoadNextLevel();
            }else{
                BattleBlasterGameInstance->RestartCurrentLevel();
            }
        }
    }

    
}


