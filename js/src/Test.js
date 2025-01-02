import { useState } from "react";
import { Joystick } from "react-joystick-component";
import { JoystickShape } from "react-joystick-component";
import "./joystick.css"

export function Test() {
  let sendRate = 50;
  let up = 15;
  let down = 11.5;
  const [joystickPos, setJoystickPos] = useState({ x: 0, y: 0 });
  const [binarySpeed, setBinarySpeed] = useState(119);
  const [currTime, setCurrTime] = useState(Date.now());
  const [max, setMax] = useState(up);


  function joystickToWheelSpeeds(x, y) {

    var leftSpeed = max;
    var rightSpeed = max;
    var magnitude = (Math.abs(y) / 10 * 6) + 0.4; // 0.4 -> 1
    var decrease = Math.abs(x * max); // 0 -> max

    if (y >= 0) {
      if (x > 0) { // right
        rightSpeed -= decrease;
        rightSpeed *= magnitude;
        leftSpeed *= magnitude;
        rightSpeed = Math.max(0, rightSpeed - 3); // 0 -> max-3
        if (max === up) {
          leftSpeed /= 1.5;
        }
      } else if (x === 0) { // forward
        rightSpeed = max;
        leftSpeed = max;
      }
      else { // left
        leftSpeed -= decrease;
        rightSpeed *= magnitude;
        leftSpeed *= magnitude;
        leftSpeed = Math.max(0, leftSpeed - 3);
        if (max === up) {
          rightSpeed /= 1.5;
        }
      }
    } else { // backwards
      leftSpeed = 0;
      rightSpeed = 1;
    }
    return {
      leftWheelSpeed: leftSpeed,
      rightWheelSpeed: rightSpeed,
    };
  }

  function setSpeed(leftWheelSpeed, rightWheelSpeed) {
    let combined8BitValue = (leftWheelSpeed << 4) | rightWheelSpeed;
    setBinarySpeed(combined8BitValue);
  }

  function callReq() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.1.249/data');
    request.send(JSON.stringify({ data: binarySpeed }))
  }

  function callStop() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.1.249/data');
    request.send(JSON.stringify({ data: 0 }))
  }

  function callEnd() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.1.249/data');
    request.send(JSON.stringify({ data: 2 }))
  }

  const handleYMove = (stick) => {
    setJoystickPos({ x: joystickPos.x, y: stick.y });
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      -joystickPos.y,
      joystickPos.x
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    if (Date.now() - currTime > sendRate) {
      setCurrTime(Date.now());
      callReq();
    }
  };

  const handleXMove = (stick) => {
    setJoystickPos({ x: stick.x, y: joystickPos.y });
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      -joystickPos.y,
      joystickPos.x
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    if (Date.now() - currTime > 100) {
      setCurrTime(Date.now());
      callReq();
    }
  };

  const handleXStop = () => {
    setJoystickPos({x: 0  , y: joystickPos.y})
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      -joystickPos.y,
      0
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    callStop();
  };

  const handleYStop = () => {
    setJoystickPos({x: joystickPos.x, y: 0})
    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      0,
      joystickPos.x
    );
    setSpeed(leftWheelSpeed, rightWheelSpeed);
    callStop();
  };

  const changeMax = () => {
    if (max === down) {
      setMax(up);
    } else {
      setMax(down);
    }
  }

  const end = () => {
    setMax(0);
    callEnd();
  }

  return (    
    <div class="grid-container">
      
      <div>
        <Joystick controlPlaneShape={JoystickShape.AxisX} pos={{x: joystickPos.x, y:0}} move={handleXMove} stop={handleXStop} stickSize={200} size={200} />
      </div>

      <div>
        <Joystick class="buttonStop" stop={end} stickSize={70} size={70}/>
      </div>

      <div>
        <Joystick controlPlaneShape={JoystickShape.AxisY} pos={{x:0, y:joystickPos.y}} move={handleYMove} stop={handleYStop} stickSize={200} size={200} />
      </div>

      <div class="divUpright">
        <Joystick stop={changeMax} stickSize={110} size={110}/>{max}
      </div>

    </div>    
  );
}