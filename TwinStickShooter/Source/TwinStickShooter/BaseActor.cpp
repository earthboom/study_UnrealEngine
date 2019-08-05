// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseActor.h"

// Sets default values
ABaseActor::ABaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseActor::LaserGageReset(void)
{
	LaserGage = 0.0f;
}

void ABaseActor::onGage(float& _f)
{
	LaserGage += _f;

	if (LaserGage > 100.0f)
		LaserGage = 100.0f;
}

//template<typename T> 
//void ABaseActor::VariableReset(T& _v)
//{
//	_v = 0;
//}