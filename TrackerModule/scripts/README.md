# Script di utilità

Questa cartella contiene una serie di script per velocizzare la compilazione e l'esecuzione di AI Watch A1. Per utilizzare gli script con un terminale Bash utilizzare questa sintassi:

```sh
sh ./nome_script.sh
```

Dove `nome_script` è il path per lo script. In alcune occasioni potrebbe essere necessari i permessi di root (usare `sudo` prima di `sh`).

**NOTA:** Questi script vanno richiamati dalla cartella principale del progetto (*AI_Watch_A1*).



Di seguito un elenco degli script e il loro scopo.

## build-and-run.sh

Esegue il build dell'applicazione e avvia il programma subito dopo se la compilazione avviene con successo.

Effettua le seguenti operazioni in sequenza:

- CMake
- Makefile
- Exec (del programma compilato)

## docker/core-cleanup.sh

Quando si termina un programma, vengono salvati dei dati all'interno della cartella build che hanno un nome dei tipo *core.XX* dove *XX* è un numero intero. Questi file possono crescere di dimensioni. Con questo script è possibile eseguire una pulizia di questi file. Potrebbero essere necessari i permessi di root.

⚠️ **ATTENZIONE:** questo script potrebbe avere risultati inaspettati, se possibile eseguire la pulizia manualmente.

## docker/create-image.sh

Crea l'immagine Docker utilizzando il Dockerfile presente nella cartella principale del progetto. Potrebbero essere necessari i permessi di root.

## docker/delete-all.sh

Elimina tutti i container, le immagini e i dati di Docker. Utile per eseguire una pulizia totale dei dati, ad esempio se si vuole recuperare un po' di spazio sul proprio computer. Potrebbero essere necessari i permessi di root.

⚠️ **ATTENZIONE:** operazione pericolosa, script da utilizzare soltanto in ambienti isolati in cui non sono presenti altri container (verificare la presenza di altri container prima di eseguirlo).

## docker/emergency-kill.sh

Termina l'esecuzione di tutti i container Docker. Da usare ad esempio quando si verificano loop infiniti nel codice e non si riesce ad uscire dal container. Potrebbero essere necessari i permessi di root.

## docker/run-container.sh

Avvia l'esecuzione di un container docker con l'immagine *tracker*. Da utilizzare solo dopo l'esecuzione di `docker/create-image.sh`.

