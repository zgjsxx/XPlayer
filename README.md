# XPlayer

[![c++](https://img.shields.io/static/v1?label=build&message=passing&color=green)](https://www.python.org/)
[![qt](https://img.shields.io/static/v1?label=qt&message=5.12.9&color=blue)](https://www.python.org/)
[![torch](https://img.shields.io/static/v1?label=ffmpeg&message=git-2020-03-15-c467328&color=blue)](https://pytorch.org/)

## 1. introduction

This is a video player based on the opensource framework ffmpeg.

Currently, it supports the below features:

- play video
- seek videos
- stop video

## 2. framework 

We have three threads, they are XDemuxThread, XAudioThread and XVideoThread

