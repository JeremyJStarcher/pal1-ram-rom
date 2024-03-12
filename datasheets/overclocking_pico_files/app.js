if (window === top) {
    try {
        document.body.addEventListener('bannerElement', event => {
            fetch(event.detail, {
                "credentials": "include",
                "method": "GET",
                "mode": "cors"
            }).then(resp => resp.text()).then(content => {
                const elm = document.getElementById('banner600percentContainer').contentDocument;
                elm.write(content);
                elm.close();
            });
        });
    } catch (ex) {
    }
}