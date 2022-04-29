Canvas {
    onPaint: {
      var context = getContext("2d");
      context.reset()
      //context.beginPath();
      
      context.moveTo(0,0)
      context.lineTo(width,0)
      
      context.moveTo(0,height)
      context.lineTo(width,height)
      
      //Draw border
      //context.lineWidth = 2
      //context.strokeStyle = "black"
      context.stroke()
      
      //Draw background
      //context.fillStyle = color
      //context.fill();
    }
  }