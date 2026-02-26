window.addEventListener('scroll', () => {
    const verticalScroll = window.scrollY;

    if (verticalScroll > 165) {
        document.querySelector('.navigation').classList.add('menu-scroll');
        document.querySelector('.scroll-to-top').classList.add('visible');
    } else {
        document.querySelector('.navigation').classList.remove('menu-scroll');
        document.querySelector('.scroll-to-top').classList.remove('visible');
    }

});

const scrollToContacts = () => {
    console.log('Scrolling to contacts');
    const contactsSection = document.querySelector('#contact');
    contactsSection.scrollIntoView({ behavior: 'smooth' });
}

const scrollToHome = () => {
    const homeSection = document.querySelector('#home');
    homeSection.scrollIntoView({ behavior: 'smooth' });
}

const openMenu = () => {
    const menu = document.querySelector('#hamburger-links');
    const hamburger = document.querySelector('#hamburger-menu');

    if (menu.classList.contains('open')) {
        menu.classList.remove('open');
        hamburger.classList.remove('fa-bars');
        hamburger.classList.add('fa-xmark');
        return;
    }
    else {
        menu.classList.add('open');
        hamburger.classList.remove('fa-xmark');
        hamburger.classList.add('fa-bars');
    }
}