# VPStageAPI
Virtual Production API

The Virtual Production API is an Uneal Engine 5 plugin designed to simplify the task of building command & control tooling for Virtual Production Stages.

It comprises as series of C++ and Blueprint functions that make it easier to build custom tools by providing a consistant interface to access common Virtual Production components suchs as nDisplay Configuration Actors, Sequences, ICVFX Cameras, color grades and more.

***NOTE:*** The functions in this API operate in Editor space and are not accesable to Actor or other game level Blueprints. They are commonly used in Editor Widgets or other editor side code.

The API is designed to operate in a UE Multi-User environment. All API calls will raise multi-user transactions sutable for nDisplay connected clients.

This is a ***BETA*** release and should be tested well before use. Please report any bugs here or hit me on socials/Discord

#### A Sample of functions exposed by the API
![VP API Sample Functions](/images/FunctionList1.png)

Ahren M