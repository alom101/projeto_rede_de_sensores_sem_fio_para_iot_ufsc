# Sistema de automação de estufa agrícola
Redes de Sensores sem Fio para IoT
25.1 
UFSC



# Dispositivos

## Tópicos MQTT

Os tópicos serão divididos hierarquicamente na seguinte estrutura:
```
estufa
|- temperatura
|   |- valor_atual
|   |- limiar
|
|- umidade_do_ar
|   |-valor_atual
|   |- limiar
|
|- umidade_do_solo
|   |-valor_atual
|
|- ventilacao
|   |-valor_atual
|
|- status
    |- versao_firmware
    |- log
    |- timestamp
```
# Servidor

Para o servidor de aplicação, usamos o broker MQTT Mosquitto para comunicação com os dispositivos e um servidor NodeRED para processamento e dashboard. Toda parte server-side foi desenvolvida em containers Docker para facilitar a replicação dos resultados obtidos.

## Pré-requisitos

Para instanciar o servidor de aplicação, precisamos dos programas `docker` e `docker-compose` e do repositório do projeto.

### Docker

[link](https://docs.docker.com/get-started/get-docker/)

### Docker-compose

Linux: Procure no seu package manager por `docker-compose`, caso não encontre, use o link abaixo.

Outros: [link](https://dockerlabs.collabnix.com/intermediate/workshop/DockerCompose/How_to_Install_Docker_Compose.html)

### Repositório

Clone este repositório com o seguinte comando
```bash
git clone https://github.com/alom101/projeto_rede_de_sensores_sem_fio_para_iot_ufsc.git
```

## Uso

```bash
cd server/
sudo docker-compose up -build #-d para rodar no background
```

NodeRED: http://localhost:1880
Mosquitto: localhost:1883

