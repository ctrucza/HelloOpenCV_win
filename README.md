# VideoPredictor: HTM on videos

## Environment
- uses OpenCV 3.0.0
- VS 2015

## Code

### Main loop (video.cpp)
- open a video file
- read each frame
- apply transformations to each frame and show them

### Using the transformations

#### Simple transformation:

``` C#
  GrayscaleTransformation g;
  TransformationDisplay grayscale("grayscale", g);
  
  // in the frame processing loop
  grayscale.display(frame);
```

#### Chaining transformations

``` C#
  GrayscaleTransformation g;
  AveragingTransformation a(width, height, 4, 4);
  ChainedTransformation c;
  c.add(&g);
  c.add(&a);
  // display as above
```

#### Writing files

``` C#
  GrayscaleTransformation g;
  VideoFileWriter w("output.avi", -1, 30, Size(width, height), false);
  ChainedTransformation c;
  c.add(&g);
  c.add(&a);
  // display as above
```

#### Combining frames
``` C#
  GrayscaleTransformation g;
  AveragingTransformation a(width, height, 4, 4);
  CombiningTransfromation c;
  c.add(g);
  c.add(a);
  VideoFileWriter w("output.avi", -1, 30, Size(width, height), false);
  
  // This transforms the source to grayscale, applies a pixelation, 
  // combines the grayscale and pixelated images in a single frame and writes it to a file
  ChainedTransformation pipeline;
  pipeline.add(&g);
  pipeline.add(&a);
  pipeline.add(&c);
  pipeline.add(&w);
```
### VideoTransformation
- apply a transformation to a frame

#### ChainedTransformation
- chain transformations

#### CombiningTransformation
- combines multiple frames (coming from other transformations) in a single frame

#### VideoFileWriter
- writes video files

#### SegmentedTransformation
- create a grid over the captured frame and apply a transformation to each element of the grid
- elements are processed in parallell

#### PredictorTransformation
- simple bigram prediction
- prepared for detecting region similarity

### TransformationDisplay
- create a window to show the result of a transformed video

