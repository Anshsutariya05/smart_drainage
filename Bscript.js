(() => {
  let latestData = null;
  
  const sendData = data => {
    fetch('http://127.0.0.1:3000/data', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data)
    }).catch(console.warn);
  };
  
  const findMonitor = () => 
    document.querySelector('.code_panel__serial-content__text, .serial-monitor-output, .js-code_editor__serial-monitor__content');
  
  const startObserver = container => {
    new MutationObserver(() => {
      try {
        const lastLine = container.innerText.trim().split('\n').pop();
        const json = lastLine?.match(/\{[^}]*\}/)?.[0];
        if (json) {
          const data = JSON.parse(json);
          if (typeof data.distance === 'number' && ['ON', 'OFF'].includes(data.motor)) {
            latestData = data;
          }
        }
      } catch {}
    }).observe(container, { childList: true, subtree: true, characterData: true });
    
    setInterval(() => {
      if (latestData) {
        console.log('Latest data:', latestData);
        sendData(latestData);
      }
    }, 3000);
  };
  
  (function init() {
    const container = findMonitor();
    container ? startObserver(container) : setTimeout(init, 5000);
  })();
})();