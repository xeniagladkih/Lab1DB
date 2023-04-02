#pragma once
struct Category
{
    int id;
    char name[20];
    char description[20];
    int productsCount;
    int productFirstAddress;
};
struct Indexer
{
    int id;
    int address;
    int exists;
};