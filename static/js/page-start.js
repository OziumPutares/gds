'use strict';

function forEach(collection, callback) {
    Array.prototype.forEach.call(collection, callback);
}

(function() {
    window.dataLayer = window.dataLayer || [];

    dataLayer.push({
        'gtm.start': new Date().getTime(),
        event:'gtm.js'
    });
})();


document.querySelector('html').classList.add('js-enabled');
