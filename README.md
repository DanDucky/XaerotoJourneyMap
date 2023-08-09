# Xaero to JourneyMap

| THIS PROJECT IS IN PROGRESS, AS IN NOT WORKING YET |
|----------------------------------------------------|

#  Xaero Format Documentation

### First, some general things:

* when I write "first bit(s)" I am reading from right to left
* I don't entirely know why some parts of the format exist, I'm just telling you how they're partitioned
* the xaero format DOES change between versions
  * THIS MEANS THAT THIS COULD BECOME OUTDATED!!! this was written as of **version 7** of the xaero region save format
* each section will have a title then a small graphic explaining it, followed by a text explanation of the logic.

## Versioning [5 Bytes]

| █ █ █ █ █ █ █ █                       | █ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █ |
|---------------------------------------|-----------------------------------------------------------------------|
| version flag<br/>`unsigned byte`      | version number <br/>`signed int`                                      |
| 255 → has version<br/>!255 → does not | read as int                                                           |

> Ok so these bytes are all versioning information.
> Each region.xaero file is written in a specific version, that version is declared in these first 5 bytes.
> 
> The first byte declares whether the file includes a version number or not. If it's 255 (all 1s) then it includes a version number in the next int.

### The following is looped over until the file is over

## Tile Chunk Number [1 Byte]
| █ █ █ █                   | █ █ █ █                   |
|---------------------------|---------------------------|
| relative<br/>X coordinate | relative<br/>Z coordinate |
| read as unsigned          | read as unsigned          |

> This is pretty self explanatory. 
> The file (region) is split into a grid of 8x8 "tile chunks" (NO THESE ARE NOT NORMAL CHUNKS). 
> Each tile chunk has a relative coord (0-7, 0-7), this is read from this part in the file. 
> Not every tile chunk may be present in the file, so you MUST read this byte.
> 
> Each tile chunk has a grid of 4x4 chunks inside of it. These 16 chunks will be listed IN ORDER in the following chunk of bytes.
> The order is most easily understandable as the following:
```cpp
for (int xChunk = 0; xChunk < 4; xChunk++) {
    for (int zChunk = 0; zChunk < 4; zChunk++) {
        readNextChunk(xChunk, zChunk);
    }
}
```

### The following is looped over 16 times

## Chunk Parameters [4 Bytes]
| █ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █                   |
|-----------------------------------------------------------------------------------------|
| read as signed int <br/>`signed int`                                                    |
| -1 → chunk is VOID, skip to next chunk<br/>else read int according to following diagram |

> First, read this as a SIGNED int. If it's equal to -1 then it means the chunk is void, and to skip to the next iteration.
> 
> Otherwise this chunk has 16x16 parameters in the following bytes, each parameter is at least 4 bytes, which includes the int you just read to check if it is void. Read the int you just read and the 256 that follow using the following information:

### The following is looped over 256 times IF the above condition is met

| █ █ █ █ █ █ █ | █                                                                                                                                                    | █ █ █ | █                                              | █ █ █ █` `█ █ █ █                            | █ █ █ █                        | █ █ | █                                                                                    | █   | █ █                             | █                                                  | █                                                      |
|---------------|------------------------------------------------------------------------------------------------------------------------------------------------------|-------|------------------------------------------------|----------------------------------------------|--------------------------------|-----|--------------------------------------------------------------------------------------|-----|---------------------------------|----------------------------------------------------|--------------------------------------------------------|
|               | Inconsistent height flag<br/>`bool`                                                                                                                  |       | Biome flag<br/>`bool`                          | Height<br/>`unsigned byte`                   | Light level<br/>`unsigned num` |     | Height flag<br/>`bool`                                                               |     | Color type<br/>`unsigned num`   | overlays flag<br/>`bool`                           | grass flag<br/>`bool`                                  |
|               | 1 → if the region version is above 4 AND this is true, then the height stored in parameters is wrong and needs to be overwritten<br/>else do nothing |       | 1 → has biome<br/>else do not read biome byte  | read as unsigned byte if height flag is true | read as 4 bit number           |     | 1 → height is not stored in parameters<br/>else it is stored outside of parameters   |     | read as 2 bit number            | 1 → has overlays<br/>else does not have overlays   | 1 → the pixel is not grass<br/>else the pixel is grass | 

> This int has information for every part of the pixel, and where to find other relevant data. The chart describes this section best.
> 
> If the chunk is not void, there will be 256 sets of these parameters to account for the entire 16x16 grid of pixels
> 
> Sections without information are unsused parts of the format

### Follow the directions for each section below if and only if the conditions in the header are met!

## `IF` Is not grass (grass flag)

| █ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █                                                                            |
|--------------------------------------------------------------------------------------------------------------------------------------------------|
| read as signed int <br/>`signed int`                                                                                                             |
| signed int is the "block state" of the pixel<br/>there's a list of all of the block state numbers and their translation to color in the codebase |

> If the parameters told us that the pixel is not grass, meaning it has a special color and block id, then we need to read this int. This int will tell us what block it is so that we can later decode its color.

## `IF` Height is not stored in parameters (height flag)

| █ █ █ █ █ █ █ █                                                                  |
|----------------------------------------------------------------------------------|
| read as unsigned byte <br/>`unsigned byte`                                       |
| unsigned byte is the height of the pixel if it is not included in the parameters |

> If the height is stored in not stored in parameters, then we need to read this byte from outside the parameters to get the height.

## `IF` Height is inconsistent (Inconsistent height flag) `AND` The region version is `GREATER THAN OR EQUAL TO` 4

| █ █ █ █ █ █ █ █                                                                                  |
|--------------------------------------------------------------------------------------------------|
| read as unsigned byte <br/>`unsigned byte`                                                       |
| unsigned byte is the top height of the pixel, which is different from the height previously read |

> I don't entirely know why "top height" is an important metric, but I guess it is
> 
> Anyways all we need to do is read this next byte to get the top height because it is different from the height stated previously
> 
> **Otherwise the top height is equal to the height read before this check**

## `IF` Has overlays (overlay flag)

> *If this is true, read the section for overlays below to handle it. This is like handling a whole other set of parameters so I didn't want to embed it here. This section will loop for as many times as there are overlays.*

## `IF` Color type is equal to 2 or 1 `OR` has biome (Biome flag)

| █ █ █ █ █ █ █ █                                                    |
|--------------------------------------------------------------------|
| biome flag or biome id<br/>`unsigned byte`                         |
| 255 → biome id is over 254<br/>else this byte is the full biome id |

> Read the next byte. If it is 255 then the biome id is a number greater than 254, meaning we can't just fit it in this single byte.
> In this case follow the instructions below.
> 
> Otherwise this byte is your biome id and you **don't** need to continue to the next if statement below.

## `IF` Biome flag is 255 `AND` region save version is `GREATER THAN OR EQUAL TO` 3
| █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █    |
|--------------------------------------------------------------------------------|
| biome id<br/>`unsigned int`                                                    |
| unsigned int is the full biome id if it is greater than or equal to 255        |

> If the biome flag we just read is 255 AND the region save version is greater than or equal to 3 then we read this next int and interpret it as the biome id.
> Ignore whatever number you just got from the previous byte!

## OVERLAYS
> **GO TO THIS SECTION AND FOLLOW THE DIRECTIONS IN IT IF HAS OVERLAYS IS TRUE!!!**
> 
> *Make sure you run the "code" from this section where I first mentioned it above, otherwise it will be running in the wrong order!*

### Number of overlays
| █ █ █ █ █ █ █ █                                                               |
|-------------------------------------------------------------------------------|
| number of overlays<br/>`unsigned byte`                                        |
| read directly as an unsigned byte to get the number of overlays on this pixel |

> This first byte is read as an unsigned byte. The number is equal to the number of overlays. Each pixel can have up to 255 overlays
> Overlays include things like water in the game, depending on the number of water overlays the color of the water might become slightly darker or lighter
> 
> Loop through the following section to decode each overlay

**THE FOLLOWING IS LOOPED THROUGH FOR AS MANY TIMES AS THERE ARE OVERLAYS**

### Overlay parameters
| █ █ █ █ █ █ █ █` `█ █ █ █ █ █ █ █` `█ █ █ █ █ █ | █ █                                                      | █ █ █ █                                                   | █                                       | █                                        | █                                            | █                                    |
|-------------------------------------------------|----------------------------------------------------------|-----------------------------------------------------------|-----------------------------------------|------------------------------------------|----------------------------------------------|--------------------------------------|
|                                                 | color type<br/>`unsigned num`                            | light level<br/>`unsigned num`                            | opacity flag<br/>`bool`                 | custom color flag<br/>`bool`             | legacy opacity flag<br/>`bool`               | water flag<br/>`bool`                |
|                                                 | read as an unsigned 2 bit number, this is the color type | read as an unsigned 4 bit number, this is the light level | 1 → has opacity > 1<br/>else does not | 1 → has custom color<br/>else does not | 1 → has a legacy opacity<br/>else does not | 1 → is not water<br/>else is water |

> This int has info for the parameters of the overlay, everything in the graph is pretty self explanatory.
> 
> Follow the directions below to decode the rest of the overlay information

### `IF` Overlay is not water (water flag)

| █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █              |
|------------------------------------------------------------------------------------------|
| overlay block id<br/>`signed int`                                                        |
| signed int is full block id of the overlay, the color of this can be grabbed from a list |

> Read this int if the block is marked as "not water" (meaning we need more space to find out what block/color it is)
> 
> This int is the block id, the color of every block id is in a list within the source of this project (it isn't yet but it will be!!!!!!)

### `IF` Overlay has a legacy opacity `AND` save version is `LESS THAN` 1

| █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █    |
|--------------------------------------------------------------------------------|
| overlay opacity<br/>`unsigned int`                                             |
| unsigned int is the opacity of the overlay pixel if it is using legacy opacity |

> Read this unsigned int as the opacity ONLY if the version is LESS THAN 1 AND the legacy opacity flag is true
> 
> This is very unlikely, especially with 2b2t data

### `IF` overlay color type is 2 `OR` overlay has custom color

| █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █                                                                               |
|-----------------------------------------------------------------------------------------------------------------------------------------------------------|
| custom color and overlay color type flag int<br/>`signed int`                                                                                             |
| -1 → color type can now be set to 0<br/>!-1 → color type can be set to 3<br/>regardless of the above conditional, this signed int is now the custom color |

> If the overlay color type is 2 OR the custom color flag is true then read this as a signed int.
> 
> If this signed int is -1 then set color type to 0, otherwise you set it to 3
> 
> Once you have set the color type, then make sure to also set overlay custom color to the value of this int

### `IF` overlay has opacity

| █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █ ` ` █ █ █ █ █ █ █ █   |
|-------------------------------------------------------------------------------|
| overlay opacity<br/>`unsigned int`                                            |
| unsigned int is the opacity of the overlay pixel                              |

> Read this as an unsigned int if the opacity flag is true. This value is now the opacity of the overlay pixel. If this bit was not set then the opacity is 1.

**THIS IS THE END OF THE 256 ITERATION LOOP I DECLARED ABOVE! THE FOLLOWING IS THE FORMAT FOR DIRECTLY AFTER EVERY PIXEL PARAMETER HAS BEEN ACCOUNTED FOR BUT BEFORE THE NEXT CHUNK PARAMETER IS WRITTEN!!!!**

## Chunk versioning

> This part just has a description, and you'll see why:
> 
> I have no clue how this works, and frankly I haven't looked into it because it's mostly used for cave data
> 
> The first byte is something to do with the version the chunk was written in... but it really doesn't matter, just skip over this data.
> 
> The next 5 bytes are all to do with cave versioning, I also skip over these.
> 
> **PLEASE NOTE: THE 5 BYTE CAVE VERSIONING INFO ONLY EXISTS FOR REGION VERSIONS ABOVE VERSION 4!!!! DO NOT SKIP OVER THIS DATA IF IT IS VERSION 4 OR BELOW OR IT WILL OFFSET YOUR DATA!!!!**

**Thank you for reading this terrible documentation, if anyone feels like rewriting it in a more clear way be my guest :3**