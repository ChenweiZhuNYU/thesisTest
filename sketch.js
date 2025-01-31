let scenes = [];
let mSerial;
let readyToReceive = false;
let media = [
  { img: "Still1Test.jpg", video: "Scene1Test.mp4" },
  { img: "Still2Test.jpg", video: "Scene2Test.mp4" },
  { img: "Still3Test.jpg", video: "Scene3Test.mp4" },
  { img: "Still4Test.jpg", video: "Scene4Test.mp4" }
];

function preload() {
  for (let i = 0; i < media.length; i++) {
    let vid = createVideo(media[i].video);
    vid.hide(); // 隐藏 HTML 视频
    vid.elt.muted = true; // 确保视频无声，避免浏览器限制
    scenes.push({
      img: loadImage(media[i].img),
      video: vid,
      playing: false
    });
  }
}

function setup() {
  createCanvas(windowWidth, windowHeight);
  mSerial = createSerial();

  // 创建串口连接按钮
  let connectButton = createButton("连接 Arduino");
  connectButton.position(20, 20);
  connectButton.mousePressed(connectToSerial);

  console.log("等待串口连接...");
}

function draw() {
  background(0);
  let sceneWidth = windowWidth / 4;
  let sceneHeight = windowHeight;

  for (let i = 0; i < scenes.length; i++) {
    let x = i * sceneWidth;
    if (scenes[i].playing) {
      image(scenes[i].video, x, 0, sceneWidth, sceneHeight);
    } else {
      image(scenes[i].img, x, 0, sceneWidth, sceneHeight);
    }
  }

  // 监听串口数据
  if (mSerial.opened() && readyToReceive) {
    readyToReceive = false;
    mSerial.clear();
    mSerial.write(0xab); // 向 Arduino 发送请求
  }

  if (mSerial.availableBytes() > 0) {
    receiveSerial();
  }
}

function playScene(index) {
  let scene = scenes[index];

  if (!scene.playing) {
    scene.playing = true;
    scene.video.play();

    scene.video.onended(() => {
      scene.playing = false;
      scene.video.hide();
    });
  }
}

function receiveSerial() {
  let line = mSerial.readUntil("\n");
  trim(line);
  if (!line) return;

  console.log("接收到数据: ", line); // 调试输出原始数据

  if (line.charAt(0) != "{") {
    console.log("数据解析错误: ", line);
    readyToReceive = true;
    return;
  }

  let json;
  try {
    json = JSON.parse(line);
  } catch (e) {
    console.error("JSON 解析失败: ", e);
    readyToReceive = true;
    return;
  }

  let data = json.data;
  console.log("解析后的数据: ", data); // 检查数据格式

  // 检测按钮状态
  for (let i = 0; i < 4; i++) {
    if (data["button" + (i + 1)]) {
      console.log("播放视频: Scene", i + 1);
      playScene(i);
    }
  }

  readyToReceive = true;
}

function connectToSerial() {
  if (!mSerial.opened()) {
    mSerial.open(9600);
    readyToReceive = true;
    console.log("串口已连接");
  }
}

function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}