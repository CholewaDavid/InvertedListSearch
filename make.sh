#!/bin/bash
g++ intersectionAlg.cpp bsrEncoder.cpp qFilter.cpp qFilterBsr.cpp shuffling.cpp shufflingBsr.cpp scalarMerge.cpp scalarMergeBsr.cpp scalarGalloping.cpp scalarGallopingBsr.cpp simdGalloping.cpp simdGallopingBsr.cpp bmiss.cpp bmissSttni.cpp hieraInter.cpp ./roaring/roaring.c roaring.cpp tester.cpp main.cpp -o main -mavx2 -fpermissive
