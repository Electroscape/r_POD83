---
title: Elancell Homepage
lang: EN/DE
group_en: browser
group_de: browser
---
<script src="https://cdnjs.cloudflare.com/ajax/libs/three.js/0.150.1/three.min.js" integrity="sha512-pHIFefLE+ZH/TmpBDxPoDyC8kBxD773LqGShSv9VSiMFLluVRKKOFO9JA+TgnQruQEKovPtZ3vEz1Hwo/F4J2A==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
<style>
#canvas, canvas {
  width: 300px;
  height: 300px;
}

.crt {
 letter-spacing: 1px;
 font-family: 'VT323', monospace;
 color: #95e208;
 -webkit-filter: blur(.2px);
 text-transform: uppercase;
 text-shadow: 0 0 5px #22ff22, 0 0 5px #aaffaa;
 font-size: 22px;
}

#monitor {
 background-color: #161913;
 
 position: relative;
 display: inline-flex;
 align-items: center;
 justify-content: center;
 width: 100%;
 height: 100%;
 text-transform: uppercase;
 padding: 15px;
}

.content {
  white-space: nowrap;
  text-align: center;
  display: inline-block;
}

.foreground, .background {
  pointer-events: none;
  position: absolute;
  left: 0;
  top: 0;
  display: block;
  width: 100%;
  height: 100%;
}

.message {
  position: absolute;
  left: 90px;
}
</style>

<div id="monitor" class="crt">
  <pre class="content crt">
  <div id="canvas"></div><br>
    ═══════[Elancell]═══════
    <br><br>Under Construction
  <span class="message"></span>

  </pre>
  <div class="foreground"></div>
</div>

<script>

/* ThreeJS */
let container = $("#canvas");
let canvasWidth = container.width();
let canvasHeight = container.height();

console.log(canvasWidth);

let scene = new THREE.Scene();
let camera = new THREE.PerspectiveCamera( 75, canvasWidth / canvasHeight, 0.1, 1000 );

let renderer = new THREE.WebGLRenderer( { alpha: true } );
renderer.setSize( canvasWidth, canvasHeight );
renderer.setClearColor( 0x000000, 0 );

container.get(0).appendChild( renderer.domElement );

let geometry = new THREE.SphereGeometry( 2, 12, 12 );
let material = new THREE.MeshBasicMaterial({
      color : 0x92ff38,
      wireframe : true,
      wireframeLinewidth: 2
    });
let cube = new THREE.Mesh( geometry, material );
scene.add( cube );

camera.position.z = 5;

function render() {
	requestAnimationFrame( render );
    cube.rotation.x += 0.0;
    cube.rotation.y += 0.01;
	renderer.render( scene, camera );
}
render();
</script>