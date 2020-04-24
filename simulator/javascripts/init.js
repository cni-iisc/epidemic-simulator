"use strict";

requirejs.config({
   
    baseUrl: 'simulator/javascripts',
    paths: {
        controller: 'controller',
        model:'model'
    }
});
require(['renderer'],function(renderer){
    "use strict";

    renderer.set_default_values_html();

})
