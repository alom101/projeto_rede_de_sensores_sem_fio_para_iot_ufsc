# Sistema de automação de estufa agrícola
Redes de Sensores sem Fio para IoT
25.1 
UFSC



# Dispositivos


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
git clone ... # <<< TODO
```

## Uso
No diretório `./server/` onde se encontra o arquivo `docker-compose.yml`, execute o seguinte comando:

```bash
sudo docker-compose up
```


