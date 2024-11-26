# AI_Watch_A1 - Documentazione e guida di utilizzo

Modulo di tracking. 

| ⚠️ **NOTA IMPORTANTE:** Intel ha deciso di rimuovere il supporto alla gestione di telecamere in remoto. Si **sconsiglia altamente** di aggiornare l'SDK alla versione più recente e utilizzare soltanto la versione 2.47.0. ⚠️ |
|:---|

**Ultima modifica alla documentazione: 21/03/2023**

Autori: Francesco Pollasto
<br><small>(aggiungi il tuo nome se modifichi)</small>


## Introduzione

Il modulo tracker prende in input i frame della telecamera RealSense e utilizza OpenPose per ottenere i dati delle persone presenti nell'immagine. Successivamente, si basa sui dati e sui dati di profondità per ricavare dei punti 3D che rappresentano le persone presenti nella scena.

Il modulo è composto di varie parti, ossia:

- **Collegamento con le telecamere** (dipendenza: RealSense SDK 2.0)

    - Recupera i dati da una RealSense collegata a un Raspberry Pi

- **OpenCV** (dipendenza)

    - Crea le immagini a partire dai dati catturati dalla RealSense

- **OpenPose** (dipendenza) e **re-identificazione** (dipendenze varie)

    - Restituisce un JSON a partire dall'immagine RGB

- **Coordinate Mapper** (dipendenza)

    - Trasforma i punti dalle coordinate dei pixel dell'immagine alle coordinate della stanza.

- **Kafka** (dipendenza)
    
    - Invia i dati agli altri moduli

- **Blockchain** (varie dipendenze)
    
    - Invia i dati su una blockchain per identificare manipolazioni

## Prerequisiti

Per installare più semplicemente le dipendenze del programma è possibile utilizzare Docker.
Per installarlo, fare riferimento a [questa guida](https://docs.docker.com/engine/install/).

Se non si vuole utilizzare Docker, installare le dipendenze manualmente sulla base dei comandi descritti nel file Dockerfile.

**Nota:** Il file CMake dell'SDK di RealSense nella versione 2.47.0 contiene un errore che causa la terminazione immediata dello strumento `make` durante la fase di compilazione di *rs-server*. Il file Dockerfile contiene una semplice patch per risolvere questo problema.

Il programma funziona meglio su Linux. È possibile utilizzarlo anche su macOS, ma è richiesta l'apertura delle seguenti porte:

- SSH: 22
- RTSP (LIVE555): 554 e 8554

Il funzionamento su macOS non è stato verificato.

Per permettere l'utilizzo della scheda grafica NVidia con OpenPose, installare NVidia Container Runtime. Fare riferimento a [questa guida](https://github.com/NVIDIA/nvidia-docker).

--- 

📢 **NOTA:** Queste istruzioni si riferiscono **soltanto** all'installazione del modulo di tracking (AI Watch A1). Per utilizzare il **programma completo** (AI Watch), riferirsi anche alla **documentazione degli altri moduli**.

---

## Installazione

Per creare l'immagine Docker spostarsi nella cartella principale del programma (quella in cui si trova questo file).
Aprire un terminale in questa cartella e eseguire il seguente comando:

```sh
sudo sh scripts/docker/create-image.sh
```

Se si vuole installare il programma normalmente, inserire i comandi presenti nel file Dockerfile. Fare attenzione a non inserire i comandi commentati, che servono soltanto per un eventuale switch futuro.

## Utilizzo

### Avvio

Avviare un container con questo comando:

```sh
sudo sh scripts/docker/run-image.sh
```

Per avviare una telecamera, collegare un Raspberry Pi con RealSense alla presa di alimentazione e attendere qualche minuto.
Poi, è possibile utilizzare due approcci:

1. collegarsi con tastiera e mouse al dispositivo
2. utilizzare SSH

Se si utilizza SSH, verificare l'indirizzo IP locale assegnato al Raspberry Pi e utilizzare questo comando su un qualsiasi computer Unix/Linux/macOS (non per forza quello su cui si sta eseguendo AI Watch A1):

```sh
ssh pi@<INDIRIZZO_IP>
```

Dove `<INDIRIZZO_IP>` va sostituito con l'indirizzo IP del dispositivo (senza parentesi angolari) e `pi`, che potrebbe variare, è il nome utente (hostname) del Raspberry Pi.

Se richiesta, inserire la password.

**NOTA:** per la password utilizzare quella del Mac Studio del laboratorio (quello in fondo a sinistra quando si entra dalla porta principale). Domandare per ottenerla.

Una volta inserita la password, si potrà controllare il Raspberry Pi da remoto.

Per avviare il server che gestisce la telecamera, utilizzare il seguente comando:

```sh
rs-server&
```

**Sul computer su cui si vuole eseguire AI Watch A1** (nel container Docker), eseguire il comando

```sh
sh scripts/build-and-run.sh
```

Il programma resterà in attesa di collegamento da parte di una telecamera.

Su **Raspberry Pi** eseguire il comando:

```sh
while ! nc <INDIRIZZO_IP_SERVER> 9943 -w0 ; do sleep 1 ; done
```

Dover `<INDIRIZZO_IP_SERVER>` è l'indirizzo IP locale del computer su cui è in esecuzione AI Watch A1. Se si utilizza un sistema diverso da Linux per eseguire il programma, utilizzare invece l'indirizzo IP del container Docker.

La telecamera si connetterà in automatico al server e il programma avvierà il processo di cattura ed elaborazione.

Se qualcosa non va, riavviare il Raspberry Pi e ripetere i passaggi descritti sopra.

### Aggiunta di dipendenze

Per aggiungere dipendenze al file C++, bisogna modificare il file *CMakeLists.txt* (qui una guida all'utilizzo).

Se si vogliono invece aggiungere dipendenze Python, inserirle in coda nel file requirements.txt. Per tenere persistenza in Docker, è possibile eseguire un `commit` oppure eliminare direttamente il container e ri-eseguire la creazione.

### Aggiunta di file .cpp o .hpp

Per permettere la compilazione di nuovi file `.cpp` o `.hpp`, è necessario aggiungerli alla fine nel file *CMakeLists.txt*.

I file `.cpp` vanno aggiunti con:

```CMake
add_library(<nome_del_modulo> <PATH_AL_FILE_CPP>)
```

dove `<nome_del_modulo>` può essere scelto a piacere.

Esempio:

```CMake
add_library(aiwatchblockchain BlockchainInteraction/BlockchainLoader.cpp BlockchainInteraction/Decorator.cpp BlockchainInteraction/Component.cpp BlockchainInteraction/BlockchainLoaderDecorator.cpp)
```

Poi, aggiungere i file anche nella lista dei file poco più sotto e infine aggiungere l'install:

```CMake
install(TARGETS <nome_del_modulo> LIBRARY DESTINATION)
```

I file `.hpp`, invece, vanno aggiunti come:

```CMake
install(FILES <PATH_AL_FILE_HPP> include)
```

**Nota:** Se i file sono in una cartella, va aggiunta anche la cartella un po' più sopra come:

```CMake
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/<PATH_ALLA_CARTELLA>")
```

## Autori dei moduli e degli strumenti

Ecco una lista delle persone che hanno lavorato a questo modulo, eventualmente per richiedere spiegazioni:

- AI Watch A1 (modulo base generale)
    - Denny Caruso (autore originale)
    - Francesco Pollasto (modifiche e aggiunte parziali)
- Blockchain
    - Stefano Verrilli (autore originale)
- OpenPose
    - Denny Caruso (versione senza re-identificazione, deprecata)
    - Mario Oliva (versione con re-identificazione)
    - Francesco Pollasto (solo modifiche per integrazione in C++)
- Script, Docker e semplificazioni per l'installazione
    - Francesco Pollasto (autore originale)
- Kafka
    - Denny Caruso (autore originale)
- Coordinate Mapping
    - Denny Caruso (autore originale)


## Link utili

- [GitHub AI Watch A1 (versione originale senza moduli aggiuntivi) di Denny Caruso](https://github.com/dennewbie/AI_Watch_A1)
- [GitHub modulo blockchain di Stefano Verrilli](https://github.com/StefanoVerrilli/AIwatch_blockchain)
- [GitHub modulo Ditto (B1) di Luca Rubino](https://github.com/Luruu/AI_Watch_B1)
- [GitHub modulo Unity (C1) di Renato Esposito](https://github.com/RenatoEsposito1999/AIWatch_C1)