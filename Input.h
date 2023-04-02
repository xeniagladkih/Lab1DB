#pragma once
#include <stdio.h>
#include <string.h>
#include "Master.h"
#include "time.h"
#include "Slave.h"
void inputCategory(struct Category *category)
{
    char name[20];
    char description[20];

    printf("Enter category's name: ");
    scanf("%s", name);
    strcpy(category->name, name);

    printf("Enter category's description: ");
    scanf("%s", description);
    strcpy(category->description, description);

}

void InputProduct(struct Product *product)
{
    char name[10];
    char description[20];
    int price;
    int weight;

    printf("Enter product's name: ");
    scanf("%s", &name);
    strcpy(product->name, name);

    printf("Enter product's description: ");
    scanf("%s", &description);
    strcpy(product->description, description);

    printf("Enter product's price: ");
    scanf("%d", &price);
    product->price = price;

    printf("Enter product's weight: ");
    scanf("%d", &weight);
    product->weight = weight;
}