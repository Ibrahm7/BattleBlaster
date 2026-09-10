// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

void ATower::BeginPlay()
{
    Super::BeginPlay();

    FTimerHandle FireTimeHandle;
    GetWorldTimerManager().SetTimer(FireTimeHandle,this,&ATower::CheckFireCondition,FireRate,true);
}

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(isInFireRange()){
        RotateTurret(Tank->GetActorLocation());
    }
}

bool ATower::isInFireRange()
{
    bool Result = false;
    if(Tank){
        float DistanceToTank = FVector::Dist(GetActorLocation(),Tank->GetActorLocation());
        Result = (DistanceToTank <= FireRange);
    }
    return Result;
}

void ATower::CheckFireCondition()
{
    if(Tank && isInFireRange()){
        Fire();
    }
}
