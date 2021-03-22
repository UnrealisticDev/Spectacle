# Spectacle

Spectacle is the first _global_ search for Unreal Engine 4 specifiers. The search is live at [Unrealistic](https://unrealistic.dev/spectacle).

![Spectacle Demo Gif](https://media.giphy.com/media/kbLFfnO7SpALgKIC6H/source.gif)

## Motivation

One of my frequent frustrations with Unreal Engine 4 is keeping track of the bevy of available specifiers. These specifiers (which I will refer to as `USpecifiers`) are enclosed in macros that follow classes, structs, enums, and other program elements to enable code reflection. There are literally hundreds of `USpecifiers`, some common and some obscure, and it is an exercise in futility to try to remember all of them. On more than one occasion, I have found myself combing through source code to hunt for specifiers.

To make matters worse, documentation for `USpecifiers` is less than comprehensive and does not always make clear how a given specifier appears or should be applied "in the wild." For example, consider the case of the `DynamicOutputParam` specifier. This incredibly useful specifier converts an output value to a type of your choosing when paired with the `DeterminesOutputType` specifier. It has been around since UE 4.6 (circa 2014!), and has probably been used by most UE4 developers at one point or another, whether they realized it or not. Indeed, this specifier features prominently in the `UGameplayStatics` function library and specifically in popular functions like `GetAllActorsOfClass` and `GetAllActorsWithInterface`. Yet seven years later, it is still absent from the official [function specifier documentation](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/GameplayArchitecture/Functions/Specifiers/index.html).

It is this problem that Spectacle was designed to solve. Its primary goal is to serve as the sole and definitive gateway for finding, tracking, and understanding `USpecifiers`, across types, versions, and sources.

## Features

The Spectacle search tool offers the following features:

* A *complete* dataset of every `USpecifier` used in the Engine source code to date (*)
* Text search with the ability to gather results for all `USpecifier` types simultaneously, including:
  * Class
  * Struct
  * Property
  * Function
  * Interface
  * Enum
  * EnumMeta
* The following data for each `USpecifier`:
  * Analysis regarding the specifier and its applications
  * Clipboard-ready syntax demo
  * Direct links to source files that most frequently contain the specifier, by Engine version

In the spirit of community (and for the sake of my Saturdays), the analysis for each `USpecifier` has been made [publicly editable](https://github.com/UnrealisticDev/Unrealistic/tree/spectacle/src/content/uspecifiers). With sufficient participation, this will make the analysis generally more comprehensive and useful for all.

> (\*) The dataset currently covers the most relevant Engine source directories, namely *Runtime*, *Editor*, and *Developer*. It does not cover the *ThirdParty* and *Program* directories, which generally did not contain `USpecifiers` based on my review. Nor does it cover the *Plugins* directory, though this may change depending on interest.

## Technical Elements

From a technical perspective, this project was implemented primarily in C++ with a brief venture into JavaScript via Node.js for the upload component. Among other things, it employs:

* The standard template library for strings, vectors, filesystem access, and exceptions
* A special-purpose C++ lexer and parser to identify `USpecifiers` from raw source code
* A parsing engine to clone and compile results for an entire Engine version
* An upload facility written in Node.js to interface with a Contentful server and store results
* Information sharing across tools using json (facilitated by this fantastic [json-for-cpp](https://github.com/nlohmann/json) library)
* An automation tool to iterate over Engine versions and monitor for the arrival of new versions

On the website front, the stack includes:

* Gatsby for static site generation
* Contentful for content delivery
* Algolia for search indexing and querying
* Netlify for continuous deployment
