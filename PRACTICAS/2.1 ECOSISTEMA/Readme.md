# Nombre del proyecto
2.1 Ecosistema: identificación de organismos con Telegram e IA en Make.com

## Descripción
El alumno toma una foto de un organismo del jardín del Tecnológico y la envía a un bot de Telegram. Un escenario de Make.com recibe el mensaje y, si trae foto, la descarga y se la pasa a un **AI Agent** con visión. El agente identifica el organismo principal de la imagen, lo clasifica como **Productor, Consumidor o Descomponedor** y explica su rol en el ecosistema. El bot responde en el mismo chat en un máximo de 35 palabras. Si el mensaje no trae foto, el bot le pide al alumno que envíe una.

## Objetivos de aprendizaje
- Configurar un disparador de Telegram (webhook) en Make.com.
- Separar el flujo con un **Router** y filtros según el tipo de mensaje (con foto o solo texto).
- Descargar un archivo de Telegram y enviarlo como entrada a un agente de IA.
- Redactar un *system prompt* que limite la respuesta a un formato breve y estructurado en español.
- Reconocer los niveles tróficos de los organismos de un ecosistema real.

## Material utilizado
- Cuenta de Make.com (zona us2.make.com) con el módulo AI Agent
- Bot de Telegram creado con @BotFather ("Ecosistema Bot")
- Conexión a un proveedor de IA con visión (modelo gpt-5-nano)
- Celular con Telegram y cámara

## Diagrama del escenario
<img src="Diagrama/Escenario.png" width="500">

```
Telegram (Watch Updates) ──► Router ─┬─ [No tiene Foto] ─► Telegram: "Envíame una foto…"
                                     │
                                     └─ [Tiene Foto] ─► Telegram: Download a File ─► AI Agent ─► Telegram: envía respuesta
```

| # | Módulo | Configuración |
|---|---|---|
| 1 | Telegram Bot – Watch Updates | Conexión: Ecosistema Bot (webhook) |
| 2 | Router | 2 rutas con filtro |
| 3 | Telegram Bot – Send a Text Message | Chat ID: `1. Message: Chat: ID` · Text: "Envíame una foto 📸 del organismo (planta, insecto, hongo, etc.) para poder identificarlo" |
| 4 | Telegram Bot – Download a File | File ID: `1. Message: Photo: File ID` |
| 5 | Make AI Agent – Run an Agent | Modelo gpt-5-nano · Input files: `4. Data` / `4. File Name` · Input: "Analiza la imagen adjunta siguiendo tus instrucciones." · Conversation ID vacío · Historial 10 · Fallback: No |
| 6 | Telegram Bot – Send a Text Message | Chat ID: `1. Message: Chat: ID` · Text: `5. Response` |

**Filtros del Router**

| Filtro | Ubicación | Condición |
|---|---|---|
| Tiene Foto | Router → Download a File | `1. Message: Photo` **Exists** |
| No tiene Foto | Router → Send a Text Message (módulo 3) | `1. Message: Photo` **Does not exist** |

## Código
📄 [SystemPrompt.txt](Codigo/SystemPrompt.txt): instrucciones del AI Agent. Hacen que el agente se enfoque en el organismo principal de la foto e ignore el fondo, como el pasto o la tierra.

## Terminal
Respuestas del bot en Telegram:

<img src="Terminal/RespuestaBot.png" width="300">

## Video del funcionamiento
▶️ [Ver video en YouTube](https://www.youtube.com/)

## Evidencias
Foto de prueba (sábila en el jardín del Tecnológico):

<img src="Resultados/Prueba_Sabila.webp" width="250">

Respuesta esperada:
```
🔎 Sábila (Aloe vera)
🌱 Productor
♻️ Realiza fotosíntesis, almacena agua y ofrece néctar a colibríes y abejas.
```

## Reporte
📑 [Reporte](Reporte/)

## Resultados
📊 [Resultados](Resultados/)

- Foto con organismo: el bot responde con el nombre, el nivel trófico y el rol.
- Foto sin organismo vivo: "❌ No identifico un organismo. Intenta con una planta, insecto u otro ser vivo."
- Solo texto: el bot pide que se envíe una foto.

## Relación con el desarrollo sustentable
Identificar a los productores, consumidores y descomponedores del jardín ayuda a entender cómo fluyen la energía y la materia en un ecosistema real y por qué hay que conservar su biodiversidad. Con la automatización, cualquier alumno puede hacer este ejercicio de campo desde su celular, sin guías impresas.

## Conclusiones
Se construyó un escenario en Make.com que conecta Telegram con un agente de IA con visión para clasificar organismos del jardín del plantel. Los filtros del Router separan los mensajes con foto de los que solo traen texto. Ajustar el *system prompt* fue clave para que el agente se enfocara en el organismo principal de la foto y no en el fondo, y para que respondiera con un formato corto y fácil de leer en el chat.
