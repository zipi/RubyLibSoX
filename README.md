# RubyLibSoX

Ruby extension to wrap LibSoX

## Still under development

The biggest outstanding issues are:
- Handle signal information so rate changes don't alter playback speed.
- Ruby implementation of EffectHandler that can be inherited, so effects can be written in Ruby.

## This is not the gem you are looking for

[SoX - Sound eXchange](http://sox.sourceforge.net/) is an amazing command line utility and it runs on every operating system, that I'm willing to use. Chances are any sound processing task you have in mind can be done quickly and easily from the command line.  For example the example0 problem from the libsox source code would be simple in the command line.
```sox input.wav output.wav vol 3dB flanger```
Using backticks or the system method it's easy to do this kind of file processing inside your ruby programs. There are also a few gems that implment a ruby interface to sox command line.

This gem, however, exposes the effects chain of soxlib so that your ruby program can see and modify the raw data of the samples.
 It's unlikely that you need to operate at this level on a sound file because all the normal things people want to do are already done and made available as an effect in the sox command line. If what you want to do is a deeper analysis of the raw data, a custom visualization, or new effect I hope this gem will make it possible to do these things in Ruby.

## Installation

Today this project is not ready to published as a gem but to get the lastest development image you can put this line in your Gemfile

gem 'ruby_libsox', git: 'git://github.com/zipi/ruby_libsox.git'

> TODO: test that github gem path

and run bundle install

## Usage

Check out the specs for all the examples of usage, these are still being adjusted as the interface evolves.

## Serious Usage

Real usage is going to involve access to the buffers and the effects chains, Since these are internals to the sox project, are not overly documented, and the Ruby inteface exposed here is unque I'll try to describe what's available here and how you might want to use it.

### Buffer

Create a new buffer with ```LibSoX.new_buffer 2048``` You can change the size to suit your purpose. If you are dealing with multi-channel sound, like stereo, make sure your size is a multiple of the number of channels

### Signal Formats

Every effect in a chain references an input and output format. Most effects just use the signal from the input file and signal from the output file, which was opened with the singal info from the input file, and never change the signal information.
Where this is not so simple is when an effect changes the signal. This happens when the rate or precision of the sample changes, or when the length changes.

Right now to handle this I use the default signals from the input and output, but allow them to be changed when adding an effect to a chain.

### Effect Handlers

Really not there yet.

## License
Copyright (c) 2015 Scott Moe - Cloudspace

Released under the MIT License


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:



The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

## Attribution

This project started as a fork of [faridco/rsox](https://github.com/faridco/rsox). I was looking for low level access to libsox from ruby and that was the best available,
but it didn't do everything I needed.
It did get me started on extending Ruby with C-lang and was very helpful. Since then I've changed this project so much it no longer much resembles the rsox project.
The most significant change was to limit the C-lang to exposing internals of libsox and providing some Ruby classes to create and API that is better suited to Ruby programs and can be tested.
So a tip of the hat to Farid Bagishev, and thanks for everything I learned from your rsox gem.
