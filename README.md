# VideoPredictor: HTM on videos

## Environment
- uses OpenCV 3.0.0
- VS 2015

## Code

### Main loop (video.cpp)
- open a video file
- read each frame
- apply transformations to each frame and show them

### VideoTransformation
- apply a transformation to a frame

### TransformationDisplay
- create a window to show the result of a transformed video

### ChainedTransformation
- chain transformations

### SegmentedTransformation
- create a grid over the captured frame and apply a transformation to each element of the grid
- elements are processed in parallell

### PredictorTransformation
- simple bigram prediction
- prepared for detecting region similarity
