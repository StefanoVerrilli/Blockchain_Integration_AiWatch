#!/bin/bash

text="[!] ATTENZIONE: se possiedi altri container o immagini, questo script eliminerà tutto. Utilizza questo script soltanto se sei in un ambiente isolato per questo progetto."
confirm="Inserisci 'y' e premi invio per continuare"

echo $text
echo $confirm
read input

if [ $input = 'y' ] 
then
    docker system prune -a --volumes
fi
