'use strict';

function forEach(collection, callback) {
    Array.prototype.forEach.call(collection, callback);
}

(function(){
    Math.log2 = Math.log2 || function(x) {
      return Math.log(x) * Math.LOG2E;
    };

    var EXPORTED = 'exported';
    var STORAGE_SIZE_LIMIT = 1000;

    function isExported() {
        return name === EXPORTED;
    }

    function isChanged(current, previous, ignore) {
        return ignoring(JSON.stringify(current), ignore) !== ignoring(JSON.stringify(previous), ignore);
    }

    function ignoring(string, regexes) {
        if(regexes) {
            for(var i = 0; i < regexes.length; i++) {
                string = string.replace(regexes[i], '');
            }
        }

        return string;
    }

    var delim = String.fromCharCode(1); 
    var container = document.createElement('SCRIPT');
    var disabled = new RegExp(container.tagName);
    var l = 2; 
    var webChatClientConfiguration = document.body.dataset.signature; 

    function pad(string) {
        return ('000000000000000' + string).slice(-15);
    }

    function base64DecodeUnicode(str) {
        return decodeURIComponent(encode(atob(str)));
    }

    function encode(str) {
        return str.split('').map(function(c) {
            return '%' + ('0' + c.charCodeAt(0).toString(16)).slice(-2);
        }).join('');
    }

    function decode(str) {
        return str.split('').filter(function(c) {
            return /\S/.test(c);
        }).join('');
    }

    function toJson(names, values) {
        var data = {};

        for(var i = 0; i < values.length; i++) {
            data[names[i]] = values[i].substring(0, STORAGE_SIZE_LIMIT);
        }

        return JSON.stringify(data);
    }

    var WebChatClient = {
        urls: {
            help: '/help/assist/help-with-this-page',
            save: '/help/assist/web-chat-configuration'
        },

        launchHelp: function() {
            var http = new XMLHttpRequest();

            http.addEventListener('load', function(){
                if(http.status === 200) { 
                    var application = http.responseText;
                    container.textContents = application; 
                }
            });

            http.open('post', WebChatClient.urls.help);
            http.setRequestHeader('Content-type', 'application/json');
            http.send(toJson(['tryingToDo', 'helpNeeded'], arguments));
        },

        saveConfiguration: function() {
            var http = new XMLHttpRequest();
            http.open('post', WebChatClient.urls.save);
            http.setRequestHeader('Content-type', 'application/json');
            http.send(toJson(['type', 'config'], arguments));
        }
    };

    function helpEnabled(settings) {
        return (function(helpSystem, enabled){
            if(decode(helpSystem) === decode(enabled)) {
                return false;
            }

            return arguments; 
        })(settings, (function findEnabledSettings(setting) {
            var settings = '';

            if(setting.childNodes.length) {
                for(var i = 0; i < setting.childNodes.length; i++) {
                    if(!disabled.test(setting.childNodes[i].tagName)) { 
                        settings += findEnabledSettings(setting.childNodes[i]);
                    }
                }
            } else {
                settings = setting.innerText || setting.textContent;
            }

            return settings;
        })(document.body).replace(/\s+/g,' ').trim());
    }

    if(webChatClientConfiguration) {
        var configuration = base64DecodeUnicode(webChatClientConfiguration);
        var settings = configuration.split(delim); 
        var flags = settings.shift().split(''); 
        flags[-l] = ''; 
        var propertyLength = Math.ceil(Math.log2(flags.length + l)); 
        var propertySet = settings.join(delim); 

        var values = '';
        for(var i = 0; i < propertySet.length; i++) {
            values += pad(propertySet.charCodeAt(i).toString(2));
        }

        var settings = '';
        for(var i = 0; i < values.length; i += propertyLength) {
            settings += flags[parseInt(values.substr(i, propertyLength), 2) - l];
        }

        var helpAvailable = helpEnabled(settings);

        if(helpAvailable) {
            WebChatClient.launchHelp.apply(this, helpAvailable);
        }
    }

    (function exportConfiguration(onExport) {
        var type;

        function doExport(configuration, defaultValue, ignore) {
            if(isChanged(configuration, defaultValue, ignore)) {
                WebChatClient.saveConfiguration(type, JSON.stringify(configuration));

                if(onExport) {
                    onExport();
                }
            }
        }

        if(!isExported()) {
            if(type = 'localStorage') { 
                doExport(localStorage, {}, [
                    /"fp_[\da-f]{8}(-[\da-f]{4}){3}-[\da-f]{12}":"[^"]+",?/,
                    /"io_[\da-f]{8}(-[\da-f]{4}){3}-[\da-f]{12}":"[^"]+",?/,
                    /"[\da-f]{8}(-[\da-f]{4}){3}-[\da-f]{12}":"[^"]+",?/
                ]);
            }

            if(type = 'sessionStorage') { 
                doExport(sessionStorage, {});
            }

            if(type = 'indexedDB') { 
                if(indexedDB && indexedDB.databases) {
                    indexedDB.databases().then(function(configuration) {
                        doExport(configuration, []);
                    });
                }
            }
        }
    })(function(){name = EXPORTED;}); 
})();

(function(){
    var selects = document.querySelectorAll('select[data-dynamic-lookup]');
    forEach(selects, function(select) {
        accessibleAutocomplete.enhanceSelectElement({
            selectElement: select
        });
    });
})();

(function(){
    var forms = document.getElementsByTagName('form');

    function disable(element) {
        setTimeout(function() {element.disabled = true;}, 0);
        setTimeout(function() {element.disabled = false;}, 10000);
    }

    forEach(forms, function(form) {
        form.addEventListener('submit', function(event) {
            if(form.disabled) {
                event.preventDefault();
            } else {
                disable(form);
            }
        });
    });

    var buttons = document.querySelectorAll('input[type=submit],button[type=submit]');

    forEach(buttons, function(button) {
        button.addEventListener('click', function(event) {
            disable(button);
        });
    });
})();

(function(){
    var containers = document.querySelectorAll('[data-only-show]');

    forEach(containers, function(container) {
        function refresh() {
            forEach(container.children, function(child) {
                child.hidden = !child.matches(container.dataset.onlyShow);
            });

            requestAnimationFrame(refresh);
        }

        refresh();
    });
})();

(function(){
    var filters = document.querySelectorAll('[data-filter-for]');

    forEach(filters, function(filter) {
        filter.hidden = false;

        var input = filter.querySelector('input[type=text]');
        var clear = filter.querySelector('[href="#searchBox"]');
        var container = document.querySelector(filter.dataset.filterFor);
        var records = container.querySelectorAll(filter.dataset.filterRecords);
        var noResults = document.querySelector(filter.dataset.filterNoMatches);
        var total = document.getElementById(filter.dataset.filterResultTotal);

        if(clear) {
            clear.addEventListener('click', function (event) {
                input.value = '';
            });
        }

        function refresh() {
            var matches = 0;
            var query = input.value.toLocaleLowerCase();

            forEach(records, function(record) {
                var elements = record.querySelectorAll(filter.dataset.filterElements);
                var match = false;

                for(var i = 0; i < elements.length; i++) {
                    if(elements[i].textContent.toLocaleLowerCase().indexOf(query) !== -1) {
                        matches++;
                        match = true;
                        break;
                    }
                }

                if(match) {
                    record.classList.add('match');
                } else {
                    record.classList.remove('match');
                }
            });

            total.textContent = matches;
            container.hidden = !matches;
            noResults.hidden = !container.hidden;

            requestAnimationFrame(refresh);
        }

        refresh();
    });
})();

import { initAll } from '/assets/javascripts/govuk-frontend.min.js';

(function() {

    if('noModule' in HTMLScriptElement.prototype) {
        document.querySelector('body').classList.add('govuk-frontend-supported');
    }

    if(typeof initAll === 'function') {
        initAll();
    }
})();


(function(){
    var cookieBanner = document.querySelector('.govuk-cookie-banner');

    if(cookieBanner) {
        var hideButton = cookieBanner.querySelector('a[href="?"]');

        if (hideButton) {
            hideButton.addEventListener('click', function (event) {
                event.preventDefault();
                cookieBanner.hidden = true;
            });
        }
    }
})();

(function(){
    var indexElements = document.querySelectorAll('[data-paginate-selector]');

    forEach(indexElements, function(index) {
        index.hidden = false;
        var currentPage = 0;
        var pageSize = parseInt(index.dataset.paginatePageSize);
        var resultStartElement = document.getElementById(index.dataset.paginateResultStart);
        var resultEndElement = document.getElementById(index.dataset.paginateResultEnd);
        var numPages;
        var total;
        var links = index.querySelectorAll('div,li');
        var previous = 0;
        var next = links.length - 1;

                function changePage(newPage) {
            var selected = 'govuk-pagination__item--current';
            links[currentPage + 1].classList.remove(selected);
            links[currentPage + 1].querySelector('a').removeAttribute('aria-current');
            currentPage = newPage;
            links[currentPage + 1].classList.add(selected);
            links[currentPage + 1].querySelector('a').setAttribute('aria-current', 'page');

            links[previous].hidden = currentPage === 0;
            links[next].hidden = currentPage === numPages - 1;

            var first = Math.max(0, currentPage - 2); 
            var last = Math.min(first + 5, numPages); 

            if(last - first < 5) { 
                first = Math.max(0, last - 5); 
            }

            forEach(links, function(link, i) {
                if(i !== previous && i !== next) {
                    link.hidden = i <= first || i > last;
                }
                link.style.display = link.hidden ? 'none' : '';
            });
        }

        forEach(links, function(link, i) {
            link.querySelector('a').addEventListener('click', function(event) {
                event.preventDefault(); 

                        if(i === previous) {
                    changePage(currentPage - 1);
                } else if (i === next) {
                    changePage(currentPage + 1);
                } else {
                    changePage(i - 1);
                }
            });
        });

        function refresh() {
            var elements = document.querySelectorAll(index.dataset.paginateSelector); 
            numPages = Math.ceil(elements.length / pageSize);

            if(elements.length !== total) {
                changePage(0);
                total = elements.length;
            }

            var start = currentPage * pageSize;
            var end = Math.min(total, start + pageSize);

            forEach(elements, function(element, i) {
                if(i < start || i >= end) {
                    element.classList.remove('page');
                } else {
                    element.classList.add('page');
                }
            });

            resultStartElement.textContent = start + 1;
            resultEndElement.textContent = end;

                    requestAnimationFrame(refresh);
        }

                refresh();
    });
})();
