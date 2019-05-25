# toy code decoder
rotation invariant toy decoder for codes in this style:

<p float="left">
  <img src="./pics/code_diff_sizes.jpg" width="200" />
</p>

#### Properties
* red and black blob define the absolute orientation
* x,y coordinate encoding: 
  * binary representation: big blobs represent 1s, small blobs represent 0s
  * Once properly rotated (red blob in the east), blobs above middle line represent x coordinate, below y coordinate

Build [![CircleCI](https://circleci.com/gh/juliangaal/hw/tree/master.svg?style=svg)](https://circleci.com/gh/juliangaal/hw/tree/master) for ubuntu 16.04, run the [installation script](./install.sh) to install all necessary components
