import { useState } from "react";
import { Joystick } from "react-joystick-component";
import "./joystick.css"

// Function to convert joystick positions to wheel speeds
function joystickToWheelSpeeds(x, y) {
  
  let leftWheelSpeed = y + x;
  let rightWheelSpeed = y - x;

  const angle = Math.atan2(y, x) / Math.PI * 180;
  console.log(angle)
  
  // Avoid division by zero in normalization
  // const angle = Math.atan2(y, x) % (Math.PI / 2);
  // var maxSpeed = Math.sqrt(2) * Math.sin(angle + Math.PI / 4);
  const maxSpeed = Math.max(Math.abs(leftWheelSpeed), Math.abs(rightWheelSpeed), 1);
  if (maxSpeed !== 0) {
    leftWheelSpeed /= maxSpeed;
    rightWheelSpeed /= maxSpeed;
  }

  return {
    leftWheelSpeed: leftWheelSpeed,
    rightWheelSpeed: rightWheelSpeed,
  };
}

// Function to map wheel speed to a 4-bit value (0-15)
function mapSpeedTo4Bit(speed) {
  return Math.round((speed + 1) * 7.5);
}

// Function to combine two 4-bit values into a single 8-bit value
function combine4BitValues(left4Bit, right4Bit) {
  return (left4Bit << 4) | right4Bit;
}



export function Test() {
  const [joystickPos, setJoystickPos] = useState({ x: 0, y: 0 });
  const [binarySpeed, setBinarySpeed] = useState(119);
  const [leftWheelSpeed, setLeftWheelSpeed] = useState(0);
  const [rightWheelSpeed, setRightWheelSpeed] = useState(0);
  const [currTime, setCurrTime] = useState(Date.now());

  function callReq() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({data : binarySpeed}))
  }

  function callStop() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({data : "01110111"}))
  }

  const handleMove = (stick) => {
    setJoystickPos({ x: stick.x, y: stick.y });

    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      stick.x,
      stick.y
    );
    
    // Convert wheel speeds to 4-bit values
    const leftWheel4Bit = mapSpeedTo4Bit(leftWheelSpeed);
    const rightWheel4Bit = mapSpeedTo4Bit(rightWheelSpeed);
    const combined8BitValue = combine4BitValues(leftWheel4Bit, rightWheel4Bit);
    setLeftWheelSpeed(((combined8BitValue >> 4) - 7) / 8.0);
    setRightWheelSpeed(((combined8BitValue & 0b00001111) - 7) / 8.0);

    //const binaryValue = combined8BitValue.toString(2).padStart(8, "0");
    setBinarySpeed(combined8BitValue);
    if (Date.now() - currTime > 1000) {
      setCurrTime(Date.now());
      callReq();
    } 
    console.log("Combined 8-bit Value (Binary):", combined8BitValue);
  };

  const handleStop = () => {
    setJoystickPos({ x: 0, y: 0 });
    setBinarySpeed("01110111");
    setLeftWheelSpeed(0);
    setRightWheelSpeed(0);
    callStop();
  };

  return (
    <div id="elem-to-center">
      <Joystick pos={joystickPos} move={handleMove} stop={handleStop} stickSize={100} size={300}/>
      <p>Binary Speed: {binarySpeed}</p>
      <p>Left Speed: {leftWheelSpeed}</p>
      <p>Right Speed: {rightWheelSpeed}</p>
    </div>
  );
}
