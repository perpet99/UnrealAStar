// Fill out your copyright notice in the Description page of Project Settings.

#include "AStarLib.h"
#include "AStarTest.h"

TArray<FVector> MyGraph::dir;

//int32 MyGraph::mapinfo[10 * 10] = {
//	0,0,0,0,0,1,0,0,0,0,
//	0,0,0,0,0,0,0,0,0,0,
//	0,0,0,0,0,1,0,0,0,0,
//	0,0,0,0,0,1,0,0,0,0,
//	0,0,0,0,0,1,0,0,0,0,
//	0,0,0,0,0,1,0,0,0,0,
//	0,0,0,0,0,0,0,0,0,0,
//	0,0,0,0,0,0,0,0,0,0,
//	0,0,0,0,0,0,0,0,0,0,
//	0,0,0,0,0,0,0,0,0,0,
//};

TSet<FMyStruct> MyGraph::NodeSet;
TMap<MyGraph::FNodeRef, FMyStruct> MyGraph::NodeMap;
UObject* MyGraph::WorldContextObject;
UWorld* MyGraph::InWorld;


float QueryFilter::GetTraversalCost(const MyGraph::FNodeRef StartNodeRef, const MyGraph::FNodeRef EndNodeRef) const
{
	auto a = MyGraph::NodeRefToVector(StartNodeRef);
	auto b = MyGraph::NodeRefToVector(EndNodeRef);

	return FVector::Distance(a, b);
}
bool QueryFilter::IsTraversalAllowed(const MyGraph::FNodeRef NodeA, const MyGraph::FNodeRef NodeB) const
{
	//if (MyGraph::mapinfo[NodeA] == 1)
	//	return false;
	//if (MyGraph::mapinfo[NodeB] == 1)
	//	return false;
	return true;
}
