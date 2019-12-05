# SousVide

## What is sous-vide?

According to Wikipedia,
> **Sous-vide**  ([/suːˈviːd/](https://en.wikipedia.org/wiki/Help:IPA/English "Help:IPA/English"); French for 'under vacuum'),[[1]](https://en.wikipedia.org/wiki/Sous-vide#cite_note-dict-1)  also known as  **low temperature long time**  (LTLT) cooking,[[2]](https://en.wikipedia.org/wiki/Sous-vide#cite_note-pmid29730528-2)[[3]](https://en.wikipedia.org/wiki/Sous-vide#cite_note-pmid30562641-3)[[4]](https://en.wikipedia.org/wiki/Sous-vide#cite_note-pmid28846600-4)  is a method of  [cooking](https://en.wikipedia.org/wiki/Cooking "Cooking")  in which food is placed in a plastic pouch or a glass jar and cooked in a water bath for longer than usual cooking times (usually 1 to 7 hours, up to 48 or more in some cases) at an accurately regulated temperature. The temperature is much lower than usually used for cooking, typically around 55 to 60 °C (131 to 140 °F) for meat, higher for vegetables. The intent is to cook the item evenly, ensuring that the inside is properly cooked without overcooking the outside, and to retain moisture.

In order to assure uniform controlled temperature on the whole container, there are some fancy (and really expensive) equipments called *thermocirculators*, which integrates water heating and pumping over a container, guarantying pretty accurate regulated temperature.

## Motivation

As said before, the *thermocirculators* are pretty expensive, even the simpler ones. For I'm a kitchen enthusiast, I've always wondered how would it be to cook something the *sous-vide* way and it seemed unreachable to me that time. But then, five minutes later I wondered *"why can't I just build one of these guys from scratch?"*. And that's what I decide to do.

I plan to spend as low money as I can on this project (yet keep it as accurate as I can), to share every project step and "releases" here on GitHub and to reach as many interested people as I can, since they can eventually help me with this.

PS: This is a side-project and will not be priority, updates may take a good while.

Anyway, **let's do it!**

## TL;DR - Just wanna make it work

(TODO)

## Releases

The idea of this section is to talk about each "release" of this project by splitting them into subsections called "bill of materials", "water circulation", "temperature control" and "monitoring and interface".

### Version "zero"
The initial release, available [here](https://github.com/gabriel-milan/SousVide/releases/tag/0.1.0), is the simplest thing I could come with. It has only a temperature sensor attached to the microcontroller and a [Blynk](https://www.blynk.cc/) app that would show me the current temperature and whether I should add hot water or not. That was it!

#### Bill of materials
* ESP32 DevKit V1;
* DS18B20 water-resistant temperature sensor;
* 4k7 resistor.

A simple connection tutorial is available [here](https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/). Thanks RandomNerdTutorials!

#### Water circulation
For the sake of simplicity on this release, water circulation is not implemented, I trusted on heat transfer only.

#### Temperature control
For controlling the water temperature, I would set the desired temperature on the Blynk app and, by monitoring it, the app would activate a LED widget (see more at the [Blynk Documentation](https://docs.blynk.cc/#widgets-displays-led)) telling me that I should add hot water. Yeah, I know, that's far from ideal, but this was the way I found of putting it to work in one morning.

#### Monitoring and interface
This was done only by the Blynk app, but the options were limited. I could see the temperature over time and the "flag" telling me to add hot water. An example screenshot of my first test is available below.

![v0.jpeg](https://raw.githubusercontent.com/gabriel-milan/SousVide/master/img/v0.jpeg)
