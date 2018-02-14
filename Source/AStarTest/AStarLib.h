// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Public/GraphAStar.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

//#include "MyAIController_AStar.generated.h"
struct FMyStruct
{

	//int32 x;
	//int32 y;

	FVector Pos;
	int Index;
	bool Block;

	TArray<int> NeighbourList;

	bool operator == (const FMyStruct& Other) const
	{
		return Pos.Equals(Other.Pos);
	}
	//friend bool operator==(const FMyStruct& a, const FMyStruct& b) {
	//	return a.Pos.X == a.Pos.X && a.Pos.Y == a.Pos.Y && a.Pos.Z == a.Pos.Z;
	//}
	//friend bool operator!=(const FMyStruct& a, const FMyStruct& b) {
	//	return a.x != b.x || a.y != b.y;
	//}

	//bool operator== (const FMyStruct& a, const FMyStruct& b)
	//{
	//	return a.x == b.x && a == b.y;
	//}

	friend uint32 GetTypeHash(const FMyStruct& Other)
	{
		return GetTypeHash(Other.Pos.X + Other.Pos.Y + Other.Pos.Z);
	}
};

const int segment = 100;
const int w = segment;
const int h = segment;



struct MyGraph {
public:
	typedef int  FNodeRef;

	//FVector	StartPos;

	static TSet<FMyStruct> NodeSet;
	static TMap<FNodeRef, FMyStruct> NodeMap;
	static UObject* WorldContextObject;
	static UWorld* InWorld;
	//TMap<FNodeRef,FMyStruct, struct FMyNode*> NodeMap;

	//static int32 mapinfo[segment * segment];

	static TArray<FVector> dir;

	static FVector NodeRefToVector(FNodeRef NodeRef) {

		return NodeMap[NodeRef].Pos;
	}

	FMyStruct *FindOrAdd(FVector pos) const
	{
		FMyStruct temp;
		temp.Pos = pos;

		auto item = NodeSet.Find(temp);

		if (item == NULL) {
			// 해당 지역 충돌 체크해봐야함 지금은 패스
			temp.Index = NodeMap.Num();
			TArray<AActor*> actorsToIgnore;
			TArray<AActor*> actorsToResult;
			float radius = 720.0f;
			ECollisionChannel traceChannel = ECC_Pawn;
			TArray<FOverlapResult> outHits;
			//TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;

			//UGlobalDefinitions::OverlapSphere(actorsToIgnore, outHits, temp.Pos, radius, traceChannel);
			TArray< TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
			//ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
			//ObjectTypes.Add(TheThingToAdd);
			//ObjectTypes.Add(TheThingToAdd2);

			//DrawDebugSphere(InWorld, temp.Pos, 20, 20, FColor::Cyan, true, 10.f);

			UKismetSystemLibrary::SphereOverlapActors(InWorld, temp.Pos, 20,
				ObjectTypes,
				//FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic)),
				NULL,
				actorsToIgnore,
				actorsToResult);

			temp.Block = (0 < actorsToResult.Num());
			if (temp.Block)
				DrawDebugSphere(InWorld, temp.Pos, 40, 20, FColor::Black, true, 10.f);
			else
				DrawDebugSphere(InWorld, temp.Pos, 20, 20, FColor::Cyan, true, 10.f);
			NodeMap.Add(temp.Index, temp);
			NodeSet.Add(temp);
		}
		return NodeSet.Find(temp);
	}

	TArray<int> GetNeighbourList(FNodeRef NodeRef)  const {
		auto mystruct = NodeMap[NodeRef];
		// 이미 리스트업한 노드이면 리턴,최소 하나는 있음
		if (mystruct.NeighbourList.Num() == 0)
		{
			for (auto item : dir) {
				//코너 필터링
				auto neighbourNode = FindOrAdd(mystruct.Pos + item);
				if (neighbourNode->Block == false)
					mystruct.NeighbourList.Add(neighbourNode->Index);
			}
		}
		return mystruct.NeighbourList;
	}

	int32 GetNeighbourCount(FNodeRef NodeRef) const {

		auto list = GetNeighbourList(NodeRef);

		return list.Num();
	}

	bool IsValidRef(FNodeRef NodeRef) const
	{
		return true;
	}
	FNodeRef GetNeighbour(const FNodeRef NodeRef, const int32 NeighbourIndex) const
	{
		auto list = GetNeighbourList(NodeRef);
		return list[NeighbourIndex];
	}

	static void MakeDirGraph() {
		dir.Add(FVector(-segment, -segment, 0));
		dir.Add(FVector(0, -segment, 0));
		dir.Add(FVector(segment, -segment, 0));
		dir.Add(FVector(segment, 0, 0));
		dir.Add(FVector(segment, segment, 0));
		dir.Add(FVector(0, segment, 0));
		dir.Add(FVector(-segment, segment, 0));
		dir.Add(FVector(-segment, 0, 0));
	}
};

class QueryFilter {
public:
	float GetHeuristicScale() const
	{
		return 1;
	}
	float GetHeuristicCost(const MyGraph::FNodeRef StartNodeRef, const MyGraph::FNodeRef EndNodeRef) const
	{
		auto a = MyGraph::NodeRefToVector(StartNodeRef);
		auto b = MyGraph::NodeRefToVector(EndNodeRef);

		return FVector::Distance(a, b);

	}
	float GetTraversalCost(const MyGraph::FNodeRef StartNodeRef, const MyGraph::FNodeRef EndNodeRef) const;
	bool IsTraversalAllowed(const MyGraph::FNodeRef NodeA, const MyGraph::FNodeRef NodeB) const;

	bool WantsPartialSolution() const
	{
		return false;
	}

};
class MyAstar {
public:
	//TArray<FMyNode> nodepool;
	MyGraph graph;

	MyAstar() {
		core = new FGraphAStar<MyGraph>(graph);
		graph.MakeDirGraph();
	}

	void Reset() {
		graph.NodeMap.Reset();
		graph.NodeSet.Reset();
	}

	FGraphAStar<MyGraph> * core;
	QueryFilter qf;
	TArray<FVector> Find(FVector start, FVector end) {

		Reset();

		start.X = FMath::FloorToInt(start.X);
		start.Y = FMath::FloorToFloat(start.Y);
		start.Z = FMath::FloorToFloat(start.Z);

		end.X = FMath::FloorToFloat(end.X);
		end.Y = FMath::FloorToFloat(end.Y);
		end.Z = FMath::FloorToFloat(end.Z);

		start.X = start.X - FMath::Fmod(start.X, segment);
		start.Y = start.Y - FMath::Fmod(start.Y, segment);
		start.Z = start.Z - FMath::Fmod(start.Z, segment);

		end.X = end.X - FMath::Fmod(end.X, segment);
		end.Y = end.Y - FMath::Fmod(end.Y, segment);
		end.Z = end.Z - FMath::Fmod(end.Z, segment);

		auto a = graph.FindOrAdd(start);
		auto b = graph.FindOrAdd(end);

		UE_LOG(LogTemp, Log, TEXT("start %s"),*start.ToString());
		UE_LOG(LogTemp, Log, TEXT("end %s"),*end.ToString());

		TArray<MyGraph::FNodeRef> OutPath;
		core->FindPath<QueryFilter>(a->Index, b->Index, qf, OutPath);

		TArray<FVector> result;

		for (auto item : OutPath) {
			auto mystruct = graph.NodeMap[item];
			result.Add(mystruct.Pos);
			DrawDebugSphere(graph.InWorld, mystruct.Pos, 30, 20, FColor::Red, true, 10.f);
		}

		return result;
	}
};