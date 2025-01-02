import React from 'react';
import { Routes, Route, BrowserRouter } from "react-router-dom";
import { Test } from './Test';

function App() {
  return (
    <>
      <BrowserRouter>
        <Routes>
          <Route path="/" element={<Test />} />
        </Routes>
      </BrowserRouter>
    </>
  )
}

export default App;
